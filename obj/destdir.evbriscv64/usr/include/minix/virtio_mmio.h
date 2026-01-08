/*
 * VirtIO MMIO transport layer for MINIX 3
 *
 * For RISC-V QEMU virt platform which uses MMIO instead of PCI
 *
 * Based on VirtIO 1.0 specification
 */

#ifndef _MINIX_VIRTIO_MMIO_H
#define _MINIX_VIRTIO_MMIO_H

#include <sys/types.h>

/*
 * VirtIO MMIO register offsets (VirtIO 1.0 / Legacy)
 */
#define VIRTIO_MMIO_MAGIC_VALUE         0x000   /* "virt" */
#define VIRTIO_MMIO_VERSION             0x004   /* Version (1 or 2) */
#define VIRTIO_MMIO_DEVICE_ID           0x008   /* Device type */
#define VIRTIO_MMIO_VENDOR_ID           0x00c   /* Vendor ID */

#define VIRTIO_MMIO_HOST_FEATURES       0x010   /* Device features */
#define VIRTIO_MMIO_HOST_FEATURES_SEL   0x014   /* Feature selector */
#define VIRTIO_MMIO_GUEST_FEATURES      0x020   /* Driver features */
#define VIRTIO_MMIO_GUEST_FEATURES_SEL  0x024   /* Feature selector */

#define VIRTIO_MMIO_GUEST_PAGE_SIZE     0x028   /* Legacy: page size */
#define VIRTIO_MMIO_QUEUE_SEL           0x030   /* Queue selector */
#define VIRTIO_MMIO_QUEUE_NUM_MAX       0x034   /* Max queue size */
#define VIRTIO_MMIO_QUEUE_NUM           0x038   /* Queue size */
#define VIRTIO_MMIO_QUEUE_ALIGN         0x03c   /* Legacy: alignment */
#define VIRTIO_MMIO_QUEUE_PFN           0x040   /* Legacy: queue PFN */

#define VIRTIO_MMIO_QUEUE_READY         0x044   /* Queue ready (v2) */
#define VIRTIO_MMIO_QUEUE_NOTIFY        0x050   /* Queue notify */
#define VIRTIO_MMIO_INTERRUPT_STATUS    0x060   /* Interrupt status */
#define VIRTIO_MMIO_INTERRUPT_ACK       0x064   /* Interrupt acknowledge */
#define VIRTIO_MMIO_STATUS              0x070   /* Device status */

/* VirtIO 1.0 (modern) queue registers */
#define VIRTIO_MMIO_QUEUE_DESC_LOW      0x080   /* Descriptor area low */
#define VIRTIO_MMIO_QUEUE_DESC_HIGH     0x084   /* Descriptor area high */
#define VIRTIO_MMIO_QUEUE_AVAIL_LOW     0x090   /* Available area low */
#define VIRTIO_MMIO_QUEUE_AVAIL_HIGH    0x094   /* Available area high */
#define VIRTIO_MMIO_QUEUE_USED_LOW      0x0a0   /* Used area low */
#define VIRTIO_MMIO_QUEUE_USED_HIGH     0x0a4   /* Used area high */

#define VIRTIO_MMIO_CONFIG_GENERATION   0x0fc   /* Config generation */
#define VIRTIO_MMIO_CONFIG              0x100   /* Device config space */

/* Magic value "virt" */
#define VIRTIO_MMIO_MAGIC               0x74726976

/* Device status bits */
#define VIRTIO_STATUS_ACK               0x01
#define VIRTIO_STATUS_DRIVER            0x02
#define VIRTIO_STATUS_DRIVER_OK         0x04
#define VIRTIO_STATUS_FEATURES_OK       0x08
#define VIRTIO_STATUS_NEEDS_RESET       0x40
#define VIRTIO_STATUS_FAILED            0x80

/* Interrupt status bits */
#define VIRTIO_MMIO_INT_VRING           0x01
#define VIRTIO_MMIO_INT_CONFIG          0x02

/* Device types */
#define VIRTIO_DEV_NET                  1
#define VIRTIO_DEV_BLK                  2
#define VIRTIO_DEV_CONSOLE              3
#define VIRTIO_DEV_RNG                  4
#define VIRTIO_DEV_BALLOON              5
#define VIRTIO_DEV_RPMSG                7
#define VIRTIO_DEV_SCSI                 8
#define VIRTIO_DEV_9P                   9
#define VIRTIO_DEV_INPUT                18
#define VIRTIO_DEV_GPU                  16

/* QEMU virt platform: VirtIO MMIO devices at 0x10001000, stride 0x1000 */
#define VIRTIO_MMIO_BASE                0x10001000UL
#define VIRTIO_MMIO_STRIDE              0x1000
#define VIRTIO_MMIO_NUM_DEVICES         8
#define VIRTIO_MMIO_IRQ_BASE            1       /* IRQs 1-8 for VirtIO */

/* Get MMIO address for device n */
#define VIRTIO_MMIO_ADDR(n)             (VIRTIO_MMIO_BASE + (n) * VIRTIO_MMIO_STRIDE)
#define VIRTIO_MMIO_IRQ(n)              (VIRTIO_MMIO_IRQ_BASE + (n))

/* Feature description */
struct virtio_feature {
    const char *name;
    u8_t bit;
    u8_t host_support;
    u8_t guest_support;
};

/* Forward declaration */
struct virtio_mmio_dev;

/*
 * Probe for a VirtIO MMIO device of given type
 * Returns device handle or NULL if not found
 */
struct virtio_mmio_dev *virtio_mmio_setup(u32_t device_type,
    const char *name, struct virtio_feature *features, int num_features,
    int threads, int skip);

/* Allocate virtqueues */
int virtio_mmio_alloc_queues(struct virtio_mmio_dev *dev, int num_queues);

/* Mark device as ready */
void virtio_mmio_device_ready(struct virtio_mmio_dev *dev);

/* Reset device */
void virtio_mmio_reset(struct virtio_mmio_dev *dev);

/* Free queues */
void virtio_mmio_free_queues(struct virtio_mmio_dev *dev);

/* Free device */
void virtio_mmio_free(struct virtio_mmio_dev *dev);

/* Feature helpers */
int virtio_mmio_host_supports(struct virtio_mmio_dev *dev, int bit);
int virtio_mmio_guest_supports(struct virtio_mmio_dev *dev, int bit);

/* Queue operations */
int virtio_mmio_to_queue(struct virtio_mmio_dev *dev, int qidx,
    struct vumap_phys *bufs, size_t num, void *data);
int virtio_mmio_from_queue(struct virtio_mmio_dev *dev, int qidx,
    void **data, size_t *len);

/* IRQ handling */
void virtio_mmio_irq_enable(struct virtio_mmio_dev *dev);
void virtio_mmio_irq_disable(struct virtio_mmio_dev *dev);
int virtio_mmio_had_irq(struct virtio_mmio_dev *dev);

/* Register access */
u32_t virtio_mmio_read32(struct virtio_mmio_dev *dev, u32_t offset);
void virtio_mmio_write32(struct virtio_mmio_dev *dev, u32_t offset, u32_t val);
u8_t virtio_mmio_config_read8(struct virtio_mmio_dev *dev, u32_t offset);
u16_t virtio_mmio_config_read16(struct virtio_mmio_dev *dev, u32_t offset);
u32_t virtio_mmio_config_read32(struct virtio_mmio_dev *dev, u32_t offset);
void virtio_mmio_config_write8(struct virtio_mmio_dev *dev, u32_t offset, u8_t val);
void virtio_mmio_config_write32(struct virtio_mmio_dev *dev, u32_t offset, u32_t val);

/*
 * Memory fence for VirtIO
 * This is one of the key new functions (~20 lines)
 */
static inline void virtio_mb(void)
{
#if defined(__riscv) || defined(__riscv__)
    __asm__ __volatile__("fence iorw, iorw" ::: "memory");
#elif defined(__arm__) || defined(__aarch64__)
    __asm__ __volatile__("dmb sy" ::: "memory");
#else
    __asm__ __volatile__("mfence" ::: "memory");
#endif
}

static inline void virtio_rmb(void)
{
#if defined(__riscv) || defined(__riscv__)
    __asm__ __volatile__("fence ir, ir" ::: "memory");
#elif defined(__arm__) || defined(__aarch64__)
    __asm__ __volatile__("dmb ld" ::: "memory");
#else
    __asm__ __volatile__("lfence" ::: "memory");
#endif
}

static inline void virtio_wmb(void)
{
#if defined(__riscv) || defined(__riscv__)
    __asm__ __volatile__("fence ow, ow" ::: "memory");
#elif defined(__arm__) || defined(__aarch64__)
    __asm__ __volatile__("dmb st" ::: "memory");
#else
    __asm__ __volatile__("sfence" ::: "memory");
#endif
}

#endif /* _MINIX_VIRTIO_MMIO_H */
