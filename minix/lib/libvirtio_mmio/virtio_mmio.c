/*
 * VirtIO MMIO transport layer implementation for MINIX 3
 *
 * For RISC-V QEMU virt platform
 */

#define _SYSTEM 1

#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/virtio_mmio.h>
#include <minix/vm.h>
#include <machine/vmparam.h>

#include "virtio_ring.h"

/* VirtIO ring structure */
struct virtio_mmio_queue {
    void *vaddr;                /* Virtual address of ring */
    phys_bytes paddr;           /* Physical address */
    u16_t num;                  /* Number of descriptors */
    u32_t ring_size;            /* Ring size in bytes */
    struct vring vring;

    u16_t free_num;             /* Free descriptors */
    u16_t free_head;            /* Next free descriptor */
    u16_t free_tail;            /* Last free descriptor */
    u16_t last_used;            /* Last used index we checked */

    void **data;                /* Opaque data pointers */
};

/* VirtIO MMIO device structure */
struct virtio_mmio_dev {
    const char *name;
    volatile u32_t *base;       /* MMIO base address */
    u32_t device_type;

    struct virtio_feature *features;
    u8_t num_features;

    struct virtio_mmio_queue *queues;
    u16_t num_queues;

    int irq;
    int irq_hook;
    int threads;
    int version;                /* VirtIO version (1 or 2) */
};

static int virtio_mmio_allow_mem(void)
{
    struct minix_mem_range mr;

    mr.mr_base = VIRTIO_MMIO_BASE;
    mr.mr_limit = VIRTIO_MMIO_BASE +
        (VIRTIO_MMIO_STRIDE * VIRTIO_MMIO_NUM_DEVICES) - 1;

    return sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr);
}

/*
 * MMIO register access
 */
u32_t virtio_mmio_read32(struct virtio_mmio_dev *dev, u32_t offset)
{
    virtio_rmb();
    return dev->base[offset / 4];
}

void virtio_mmio_write32(struct virtio_mmio_dev *dev, u32_t offset, u32_t val)
{
    dev->base[offset / 4] = val;
    virtio_wmb();
}

/*
 * Config space access
 */
u8_t virtio_mmio_config_read8(struct virtio_mmio_dev *dev, u32_t offset)
{
    volatile u8_t *p = (volatile u8_t *)dev->base;
    return p[VIRTIO_MMIO_CONFIG + offset];
}

u16_t virtio_mmio_config_read16(struct virtio_mmio_dev *dev, u32_t offset)
{
    volatile u16_t *p = (volatile u16_t *)dev->base;
    return p[(VIRTIO_MMIO_CONFIG + offset) / 2];
}

u32_t virtio_mmio_config_read32(struct virtio_mmio_dev *dev, u32_t offset)
{
    return dev->base[(VIRTIO_MMIO_CONFIG + offset) / 4];
}

void virtio_mmio_config_write8(struct virtio_mmio_dev *dev, u32_t offset, u8_t val)
{
    volatile u8_t *p = (volatile u8_t *)dev->base;
    p[VIRTIO_MMIO_CONFIG + offset] = val;
}

void virtio_mmio_config_write32(struct virtio_mmio_dev *dev, u32_t offset, u32_t val)
{
    dev->base[(VIRTIO_MMIO_CONFIG + offset) / 4] = val;
}

/*
 * Exchange features with device
 */
static int exchange_features(struct virtio_mmio_dev *dev)
{
    u32_t host_features_lo, host_features_hi;
    u32_t guest_features_lo = 0;
    u32_t guest_features_hi = 0;
    int i;

    /* Read host features */
    virtio_mmio_write32(dev, VIRTIO_MMIO_HOST_FEATURES_SEL, 0);
    host_features_lo = virtio_mmio_read32(dev, VIRTIO_MMIO_HOST_FEATURES);
    virtio_mmio_write32(dev, VIRTIO_MMIO_HOST_FEATURES_SEL, 1);
    host_features_hi = virtio_mmio_read32(dev, VIRTIO_MMIO_HOST_FEATURES);

    /* Check which features we support */
    for (i = 0; i < dev->num_features; i++) {
        u8_t bit = dev->features[i].bit;
        if (bit < 32) {
            if (host_features_lo & (1U << bit)) {
                dev->features[i].host_support = 1;
                if (dev->features[i].guest_support)
                    guest_features_lo |= (1U << bit);
            }
        } else if (bit < 64) {
            u8_t shift = bit - 32;
            if (host_features_hi & (1U << shift)) {
                dev->features[i].host_support = 1;
                if (dev->features[i].guest_support)
                    guest_features_hi |= (1U << shift);
            }
        }
    }

    /* VirtIO 1.0 devices require VIRTIO_F_VERSION_1 (bit 32). */
    if (dev->version >= 2 && (host_features_hi & 1U))
        guest_features_hi |= 1U;

    /* Write guest features */
    virtio_mmio_write32(dev, VIRTIO_MMIO_GUEST_FEATURES_SEL, 0);
    virtio_mmio_write32(dev, VIRTIO_MMIO_GUEST_FEATURES, guest_features_lo);
    virtio_mmio_write32(dev, VIRTIO_MMIO_GUEST_FEATURES_SEL, 1);
    virtio_mmio_write32(dev, VIRTIO_MMIO_GUEST_FEATURES, guest_features_hi);

    return OK;
}

/*
 * Allocate a virtqueue
 */
static int alloc_queue(struct virtio_mmio_dev *dev, int qidx)
{
    struct virtio_mmio_queue *q = &dev->queues[qidx];
    u32_t num_max;
    size_t ring_size;
    void *mem;
    phys_bytes phys;
    int i;

    /* Select queue */
    virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_SEL, qidx);

    /* Get max queue size */
    num_max = virtio_mmio_read32(dev, VIRTIO_MMIO_QUEUE_NUM_MAX);
    if (num_max == 0)
        return EINVAL;

    /* Use smaller of max and 256 */
    q->num = (num_max < 256) ? num_max : 256;

    /* Calculate ring size */
    ring_size = vring_size(q->num, PAGE_SIZE);
    q->ring_size = ring_size;

    /* Allocate contiguous memory for ring */
    mem = alloc_contig(ring_size, AC_ALIGN4K, &phys);
    if (mem == NULL)
        return ENOMEM;

    memset(mem, 0, ring_size);
    q->vaddr = mem;
    q->paddr = phys;

    /* Initialize vring */
    vring_init(&q->vring, q->num, mem, PAGE_SIZE);

    /* Initialize free list */
    q->free_num = q->num;
    q->free_head = 0;
    q->free_tail = q->num - 1;
    q->last_used = 0;

    for (i = 0; i < q->num; i++) {
        q->vring.desc[i].next = (i + 1) % q->num;
    }

    /* Allocate data pointer array */
    q->data = malloc(q->num * sizeof(void *));
    if (q->data == NULL) {
        free_contig(mem, ring_size);
        return ENOMEM;
    }
    memset(q->data, 0, q->num * sizeof(void *));

    /* Configure queue in device */
    virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_NUM, q->num);

    if (dev->version == 1) {
        /* Legacy interface */
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_ALIGN, PAGE_SIZE);
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_PFN, phys / PAGE_SIZE);
    } else {
        /* Modern interface */
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_DESC_LOW, (u32_t)phys);
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_DESC_HIGH, (u32_t)(phys >> 32));
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_AVAIL_LOW,
            (u32_t)(phys + q->num * sizeof(struct vring_desc)));
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_AVAIL_HIGH, 0);
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_USED_LOW,
            (u32_t)(phys + ring_size - q->num * sizeof(struct vring_used_elem) - 6));
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_USED_HIGH, 0);
        virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_READY, 1);
    }

    return OK;
}

/*
 * Setup a VirtIO MMIO device
 */
struct virtio_mmio_dev *virtio_mmio_setup(u32_t device_type,
    const char *name, struct virtio_feature *features, int num_features,
    int threads, int skip)
{
    struct virtio_mmio_dev *dev;
    volatile u32_t *base;
    u32_t magic, version, devid;
    int i, found = 0;
    phys_bytes mmio_addr;
    int r;

    if (name == NULL || threads <= 0)
        return NULL;

    r = virtio_mmio_allow_mem();
    if (r != OK) {
        printf("%s: unable to add mmio mem range (%d)\n", name, r);
        return NULL;
    }

    /* Scan for device */
    for (i = 0; i < VIRTIO_MMIO_NUM_DEVICES && !found; i++) {
        mmio_addr = VIRTIO_MMIO_ADDR(i);

        /* Map MMIO region */
        base = (volatile u32_t *)vm_map_phys(SELF, (void *)mmio_addr, PAGE_SIZE);
        if (base == MAP_FAILED)
            continue;

        /* Check magic */
        magic = base[VIRTIO_MMIO_MAGIC_VALUE / 4];
        if (magic != VIRTIO_MMIO_MAGIC) {
            vm_unmap_phys(SELF, (void *)base, PAGE_SIZE);
            continue;
        }

        /* Check device ID */
        devid = base[VIRTIO_MMIO_DEVICE_ID / 4];
        if (devid == device_type) {
            if (skip > 0) {
                skip--;
                vm_unmap_phys(SELF, (void *)base, PAGE_SIZE);
                continue;
            }
            found = 1;
        } else {
            vm_unmap_phys(SELF, (void *)base, PAGE_SIZE);
        }
    }

    if (!found)
        return NULL;

    /* Allocate device structure */
    dev = malloc(sizeof(*dev));
    if (dev == NULL) {
        vm_unmap_phys(SELF, (void *)base, PAGE_SIZE);
        return NULL;
    }

    memset(dev, 0, sizeof(*dev));
    dev->name = name;
    dev->base = base;
    dev->device_type = device_type;
    dev->features = features;
    dev->num_features = num_features;
    dev->threads = threads;
    dev->irq = VIRTIO_MMIO_IRQ(i - 1);

    /* Get version */
    version = base[VIRTIO_MMIO_VERSION / 4];
    dev->version = version;

    /* Reset device */
    virtio_mmio_write32(dev, VIRTIO_MMIO_STATUS, 0);

    /* Acknowledge device */
    virtio_mmio_write32(dev, VIRTIO_MMIO_STATUS, VIRTIO_STATUS_ACK);

    /* Tell device we are a driver */
    virtio_mmio_write32(dev, VIRTIO_MMIO_STATUS,
        VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER);

    /* Set page size for legacy devices */
    if (version == 1) {
        virtio_mmio_write32(dev, VIRTIO_MMIO_GUEST_PAGE_SIZE, PAGE_SIZE);
    }

    /* Exchange features */
    if (exchange_features(dev) != OK) {
        virtio_mmio_free(dev);
        return NULL;
    }

    /* For v2, set FEATURES_OK */
    if (version >= 2) {
        u32_t status = virtio_mmio_read32(dev, VIRTIO_MMIO_STATUS);
        virtio_mmio_write32(dev, VIRTIO_MMIO_STATUS,
            status | VIRTIO_STATUS_FEATURES_OK);

        /* Check if device accepted features */
        status = virtio_mmio_read32(dev, VIRTIO_MMIO_STATUS);
        if (!(status & VIRTIO_STATUS_FEATURES_OK)) {
            virtio_mmio_free(dev);
            return NULL;
        }
    }

    return dev;
}

/*
 * Allocate queues
 */
int virtio_mmio_alloc_queues(struct virtio_mmio_dev *dev, int num_queues)
{
    int i, r;

    dev->queues = malloc(num_queues * sizeof(struct virtio_mmio_queue));
    if (dev->queues == NULL)
        return ENOMEM;

    memset(dev->queues, 0, num_queues * sizeof(struct virtio_mmio_queue));
    dev->num_queues = num_queues;

    for (i = 0; i < num_queues; i++) {
        r = alloc_queue(dev, i);
        if (r != OK) {
            virtio_mmio_free_queues(dev);
            return r;
        }
    }

    return OK;
}

/*
 * Mark device as ready
 */
void virtio_mmio_device_ready(struct virtio_mmio_dev *dev)
{
    u32_t status;

    /* Register IRQ */
    dev->irq_hook = dev->irq;
    sys_irqsetpolicy(dev->irq, 0, &dev->irq_hook);
    sys_irqenable(&dev->irq_hook);

    /* Set DRIVER_OK */
    status = virtio_mmio_read32(dev, VIRTIO_MMIO_STATUS);
    virtio_mmio_write32(dev, VIRTIO_MMIO_STATUS, status | VIRTIO_STATUS_DRIVER_OK);
}

/*
 * Reset device
 */
void virtio_mmio_reset(struct virtio_mmio_dev *dev)
{
    if (dev->irq_hook >= 0) {
        sys_irqdisable(&dev->irq_hook);
        sys_irqrmpolicy(&dev->irq_hook);
        dev->irq_hook = -1;
    }

    virtio_mmio_write32(dev, VIRTIO_MMIO_STATUS, 0);
}

/*
 * Free queues
 */
void virtio_mmio_free_queues(struct virtio_mmio_dev *dev)
{
    int i;

    if (dev->queues == NULL)
        return;

    for (i = 0; i < dev->num_queues; i++) {
        struct virtio_mmio_queue *q = &dev->queues[i];
        if (q->vaddr != NULL) {
            free_contig(q->vaddr, q->ring_size);
        }
        if (q->data != NULL) {
            free(q->data);
        }
    }

    free(dev->queues);
    dev->queues = NULL;
    dev->num_queues = 0;
}

/*
 * Free device
 */
void virtio_mmio_free(struct virtio_mmio_dev *dev)
{
    if (dev == NULL)
        return;

    virtio_mmio_reset(dev);
    virtio_mmio_free_queues(dev);

    if (dev->base != NULL) {
        vm_unmap_phys(SELF, (void *)dev->base, PAGE_SIZE);
    }

    free(dev);
}

/*
 * Check if host supports feature
 */
int virtio_mmio_host_supports(struct virtio_mmio_dev *dev, int bit)
{
    int i;
    for (i = 0; i < dev->num_features; i++) {
        if (dev->features[i].bit == bit)
            return dev->features[i].host_support;
    }
    return 0;
}

/*
 * Check if guest supports feature
 */
int virtio_mmio_guest_supports(struct virtio_mmio_dev *dev, int bit)
{
    int i;
    for (i = 0; i < dev->num_features; i++) {
        if (dev->features[i].bit == bit)
            return dev->features[i].guest_support;
    }
    return 0;
}

/*
 * Add buffers to queue
 */
int virtio_mmio_to_queue(struct virtio_mmio_dev *dev, int qidx,
    struct vumap_phys *bufs, size_t num, void *data)
{
    struct virtio_mmio_queue *q = &dev->queues[qidx];
    u16_t head, idx;
    size_t i;

    if (num > q->free_num)
        return EAGAIN;

    head = q->free_head;
    idx = head;

    /* Build descriptor chain */
    for (i = 0; i < num; i++) {
        struct vring_desc *d = &q->vring.desc[idx];

        d->addr = bufs[i].vp_addr & ~1UL;
        d->len = bufs[i].vp_size;
        d->flags = 0;

        if (bufs[i].vp_addr & 1)
            d->flags |= VRING_DESC_F_WRITE;

        if (i < num - 1) {
            d->flags |= VRING_DESC_F_NEXT;
            idx = d->next;
        }
    }

    /* Update free list */
    q->free_head = q->vring.desc[idx].next;
    q->free_num -= num;

    /* Store data pointer */
    q->data[head] = data;

    /* Add to available ring */
    virtio_wmb();
    q->vring.avail->ring[q->vring.avail->idx % q->num] = head;
    virtio_wmb();
    q->vring.avail->idx++;
    virtio_wmb();

    /* Notify device */
    virtio_mmio_write32(dev, VIRTIO_MMIO_QUEUE_NOTIFY, qidx);

    return OK;
}

/*
 * Get completed buffers from queue
 */
int virtio_mmio_from_queue(struct virtio_mmio_dev *dev, int qidx,
    void **data, size_t *len)
{
    struct virtio_mmio_queue *q = &dev->queues[qidx];
    struct vring_used_elem *e;
    u16_t idx, head;

    virtio_rmb();

    if (q->last_used == q->vring.used->idx)
        return -1;

    idx = q->last_used % q->num;
    e = &q->vring.used->ring[idx];
    head = e->id;

    *data = q->data[head];
    if (len != NULL)
        *len = e->len;

    q->data[head] = NULL;

    /* Return descriptors to free list */
    idx = head;
    while (q->vring.desc[idx].flags & VRING_DESC_F_NEXT) {
        q->free_num++;
        idx = q->vring.desc[idx].next;
    }
    q->free_num++;
    q->vring.desc[idx].next = q->free_head;
    q->free_head = head;

    q->last_used++;

    return 0;
}

/*
 * IRQ handling
 */
void virtio_mmio_irq_enable(struct virtio_mmio_dev *dev)
{
    sys_irqenable(&dev->irq_hook);
}

void virtio_mmio_irq_disable(struct virtio_mmio_dev *dev)
{
    sys_irqdisable(&dev->irq_hook);
}

int virtio_mmio_had_irq(struct virtio_mmio_dev *dev)
{
    u32_t status = virtio_mmio_read32(dev, VIRTIO_MMIO_INTERRUPT_STATUS);

    if (status) {
        virtio_mmio_write32(dev, VIRTIO_MMIO_INTERRUPT_ACK, status);
        return 1;
    }

    return 0;
}
