/*
 * VirtIO block driver for MINIX 3 (MMIO transport)
 *
 * For RISC-V QEMU virt platform
 */

#include <assert.h>
#include <minix/drivers.h>
#include <minix/blockdriver_mt.h>
#include <minix/drvlib.h>
#include <minix/virtio_mmio.h>
#include <minix/sysutil.h>
#include <sys/ioc_disk.h>

#include "virtio_blk.h"

#define mystatus(tid)  (status_vir[(tid)] & 0xFF)

#define dprintf(s) do {                     \
    printf("%s: ", name);                   \
    printf s;                               \
    printf("\n");                           \
} while (0)

/* Number of threads */
#define VIRTIO_BLK_NUM_THREADS      4

/* Block size */
#define VIRTIO_BLK_BLOCK_SIZE       512

static const char *const name = "virtio-blk-mmio";

/* Device handle */
static struct virtio_mmio_dev *blk_dev;

/* Block config */
static struct virtio_blk_config blk_config;

/* Feature table */
struct virtio_feature blkf[] = {
    { "barrier",    VIRTIO_BLK_F_BARRIER,   0, 0 },
    { "sizemax",    VIRTIO_BLK_F_SIZE_MAX,  0, 0 },
    { "segmax",     VIRTIO_BLK_F_SEG_MAX,   0, 0 },
    { "geometry",   VIRTIO_BLK_F_GEOMETRY,  0, 0 },
    { "read-only",  VIRTIO_BLK_F_RO,        0, 0 },
    { "blocksize",  VIRTIO_BLK_F_BLK_SIZE,  0, 0 },
    { "flush",      VIRTIO_BLK_F_FLUSH,     0, 0 },
};

/* State */
static int spurious_interrupt = 0;
static int terminating = 0;
static int open_count = 0;

/* Partitions */
struct device part[DEV_PER_DRIVE];
struct device subpart[SUB_PER_DRIVE];

/* Request headers */
static struct virtio_blk_outhdr *hdrs_vir;
static phys_bytes hdrs_phys;

/* Status bytes */
static u16_t *status_vir;
static phys_bytes status_phys;

/* Prototypes */
static int virtio_blk_open(devminor_t minor, int access);
static int virtio_blk_close(devminor_t minor);
static ssize_t virtio_blk_transfer(devminor_t minor, int write, u64_t position,
    endpoint_t endpt, iovec_t *iovec, unsigned int cnt, int flags);
static int virtio_blk_ioctl(devminor_t minor, unsigned long req,
    endpoint_t endpt, cp_grant_id_t grant, endpoint_t user_endpt);
static struct device *virtio_blk_part(devminor_t minor);
static void virtio_blk_geometry(devminor_t minor, struct part_geom *entry);
static void virtio_blk_intr(unsigned int irqs);

static struct blockdriver virtio_blk_tab = {
    .bdr_type       = BLOCKDRIVER_TYPE_DISK,
    .bdr_open       = virtio_blk_open,
    .bdr_close      = virtio_blk_close,
    .bdr_transfer   = virtio_blk_transfer,
    .bdr_ioctl      = virtio_blk_ioctl,
    .bdr_part       = virtio_blk_part,
    .bdr_geometry   = virtio_blk_geometry,
    .bdr_intr       = virtio_blk_intr,
};

/*
 * Allocate request buffers
 */
static int virtio_blk_alloc_requests(void)
{
    hdrs_vir = alloc_contig(VIRTIO_BLK_NUM_THREADS * sizeof(*hdrs_vir),
        AC_ALIGN4K, &hdrs_phys);
    if (hdrs_vir == NULL)
        return ENOMEM;

    status_vir = alloc_contig(VIRTIO_BLK_NUM_THREADS * sizeof(*status_vir),
        AC_ALIGN4K, &status_phys);
    if (status_vir == NULL) {
        free_contig(hdrs_vir, VIRTIO_BLK_NUM_THREADS * sizeof(*hdrs_vir));
        return ENOMEM;
    }

    return OK;
}

/*
 * Free request buffers
 */
static void virtio_blk_free_requests(void)
{
    if (hdrs_vir != NULL) {
        free_contig(hdrs_vir, VIRTIO_BLK_NUM_THREADS * sizeof(*hdrs_vir));
        hdrs_vir = NULL;
    }
    if (status_vir != NULL) {
        free_contig(status_vir, VIRTIO_BLK_NUM_THREADS * sizeof(*status_vir));
        status_vir = NULL;
    }
}

/*
 * Read device config
 */
static void virtio_blk_read_config(void)
{
    int i;
    u8_t *p = (u8_t *)&blk_config;

    for (i = 0; i < sizeof(blk_config); i++) {
        p[i] = virtio_mmio_config_read8(blk_dev, i);
    }

    dprintf(("capacity: %llu sectors", (unsigned long long)blk_config.capacity));
}

/*
 * Initialize device
 */
static int virtio_blk_init(void)
{
    int r;

    /* Find device */
    blk_dev = virtio_mmio_setup(VIRTIO_DEV_BLK, name, blkf,
        sizeof(blkf) / sizeof(blkf[0]), VIRTIO_BLK_NUM_THREADS, 0);

    if (blk_dev == NULL) {
        dprintf(("device not found"));
        return ENODEV;
    }

    /* Allocate queue */
    r = virtio_mmio_alloc_queues(blk_dev, 1);
    if (r != OK) {
        dprintf(("failed to allocate queues"));
        virtio_mmio_free(blk_dev);
        return r;
    }

    /* Allocate request buffers */
    r = virtio_blk_alloc_requests();
    if (r != OK) {
        dprintf(("failed to allocate requests"));
        virtio_mmio_free_queues(blk_dev);
        virtio_mmio_free(blk_dev);
        return r;
    }

    /* Read config */
    virtio_blk_read_config();

    /* Start device */
    virtio_mmio_device_ready(blk_dev);

    dprintf(("initialized"));

    return OK;
}

/*
 * Open device
 */
static int virtio_blk_open(devminor_t minor, int access)
{
    struct device *dev;

    if (terminating)
        return EINVAL;

    dev = virtio_blk_part(minor);
    if (dev == NULL)
        return ENXIO;

    if (access & BDEV_W_BIT) {
        if (virtio_mmio_host_supports(blk_dev, VIRTIO_BLK_F_RO))
            return EACCES;
    }

    if (open_count == 0) {
        /* First open - read partition table */
        partition(&virtio_blk_tab, 0, P_PRIMARY, 0);
    }

    open_count++;
    return OK;
}

/*
 * Close device
 */
static int virtio_blk_close(devminor_t minor)
{
    if (open_count > 0)
        open_count--;
    return OK;
}

/*
 * Transfer data
 */
static ssize_t virtio_blk_transfer(devminor_t minor, int write, u64_t position,
    endpoint_t endpt, iovec_t *iovec, unsigned int cnt, int flags)
{
    struct device *dev;
    struct vumap_phys phys[3];
    u64_t sector;
    size_t size, total;
    thread_id_t tid;
    int r;
    void *data;

    dev = virtio_blk_part(minor);
    if (dev == NULL)
        return ENXIO;

    if (write && virtio_mmio_host_supports(blk_dev, VIRTIO_BLK_F_RO))
        return EACCES;

    tid = blockdriver_mt_get_tid();

    /* Calculate sector */
    sector = (dev->dv_base + position) / VIRTIO_BLK_BLOCK_SIZE;

    /* Setup header */
    hdrs_vir[tid].type = write ? VIRTIO_BLK_T_OUT : VIRTIO_BLK_T_IN;
    hdrs_vir[tid].reserved = 0;
    hdrs_vir[tid].sector = sector;

    /* Setup status */
    status_vir[tid] = write ? 0 : 1;  /* 1 = writable */

    /* Map user buffer */
    total = 0;
    for (unsigned int i = 0; i < cnt; i++) {
        total += iovec[i].iov_size;
    }

    /* TODO: Proper scatter-gather support */
    /* For now, just handle single buffer case */

    /* Queue: header, data, status */
    phys[0].vp_addr = hdrs_phys + tid * sizeof(*hdrs_vir);
    phys[0].vp_size = sizeof(struct virtio_blk_outhdr);

    /* Data buffer - need to map from user space */
    /* Simplified: assume single contiguous buffer */
    phys[1].vp_addr = 0;  /* TODO: Get physical address of user buffer */
    phys[1].vp_size = total;
    if (!write)
        phys[1].vp_addr |= 1;  /* Mark as writable */

    phys[2].vp_addr = (status_phys + tid * sizeof(*status_vir)) | 1;
    phys[2].vp_size = sizeof(u8_t);

    /* Submit request */
    r = virtio_mmio_to_queue(blk_dev, 0, phys, 3, (void *)(uintptr_t)tid);
    if (r != OK)
        return r;

    /* Wait for completion */
    while (virtio_mmio_from_queue(blk_dev, 0, &data, NULL) != 0) {
        /* TODO: Proper waiting */
    }

    /* Check status */
    if (mystatus(tid) != VIRTIO_BLK_S_OK)
        return EIO;

    return total;
}

/*
 * IOCTL
 */
static int virtio_blk_ioctl(devminor_t minor, unsigned long req,
    endpoint_t endpt, cp_grant_id_t grant, endpoint_t user_endpt)
{
    switch (req) {
    case DIOCOPENCT:
        return sys_safecopyto(endpt, grant, 0, (vir_bytes)&open_count,
            sizeof(open_count));

    default:
        return ENOTTY;
    }
}

/*
 * Get partition
 */
static struct device *virtio_blk_part(devminor_t minor)
{
    if (minor < 0)
        return NULL;

    if (minor < DEV_PER_DRIVE)
        return &part[minor];

    minor -= DEV_PER_DRIVE;
    if (minor < SUB_PER_DRIVE)
        return &subpart[minor];

    return NULL;
}

/*
 * Get geometry
 */
static void virtio_blk_geometry(devminor_t minor, struct part_geom *entry)
{
    entry->cylinders = blk_config.geometry.cylinders;
    entry->heads = blk_config.geometry.heads;
    entry->sectors = blk_config.geometry.sectors;

    if (entry->cylinders == 0)
        entry->cylinders = blk_config.capacity / (16 * 63);
    if (entry->heads == 0)
        entry->heads = 16;
    if (entry->sectors == 0)
        entry->sectors = 63;
}

/*
 * Interrupt handler
 */
static void virtio_blk_intr(unsigned int irqs)
{
    if (!virtio_mmio_had_irq(blk_dev)) {
        spurious_interrupt++;
        return;
    }

    /* Re-enable IRQ */
    virtio_mmio_irq_enable(blk_dev);
}

/*
 * SEF init
 */
static int sef_cb_init(int type, sef_init_info_t *info)
{
    int r;

    r = virtio_blk_init();
    if (r != OK)
        return r;

    /* Setup partition table */
    memset(part, 0, sizeof(part));
    memset(subpart, 0, sizeof(subpart));

    part[0].dv_base = 0;
    part[0].dv_size = blk_config.capacity * VIRTIO_BLK_BLOCK_SIZE;

    blockdriver_announce(type);

    return OK;
}

static void sef_cb_signal(int signo)
{
    if (signo == SIGTERM) {
        terminating = 1;
    }
}

void sef_startup(void)
{
    sef_setcb_init_fresh(sef_cb_init);
    sef_setcb_signal_handler(sef_cb_signal);
    sef_startup_default();
}

/*
 * Main
 */
int main(void)
{
    sef_startup();
    blockdriver_mt_task(&virtio_blk_tab);
    return 0;
}
