/*
 * VirtIO block device definitions
 */

#ifndef _VIRTIO_BLK_H
#define _VIRTIO_BLK_H

/* Feature bits */
#define VIRTIO_BLK_F_BARRIER    0   /* Supports barrier */
#define VIRTIO_BLK_F_SIZE_MAX   1   /* Max segment size */
#define VIRTIO_BLK_F_SEG_MAX    2   /* Max segments */
#define VIRTIO_BLK_F_GEOMETRY   4   /* Has geometry */
#define VIRTIO_BLK_F_RO         5   /* Read-only */
#define VIRTIO_BLK_F_BLK_SIZE   6   /* Has block size */
#define VIRTIO_BLK_F_SCSI       7   /* SCSI commands */
#define VIRTIO_BLK_F_FLUSH      9   /* Flush command */
#define VIRTIO_BLK_F_TOPOLOGY   10  /* Topology info */

/* Config structure */
struct virtio_blk_config {
    u64_t capacity;             /* Size in 512-byte sectors */
    u32_t size_max;             /* Max segment size */
    u32_t seg_max;              /* Max segments */
    struct {
        u16_t cylinders;
        u8_t heads;
        u8_t sectors;
    } geometry;
    u32_t blk_size;             /* Block size */
    struct {
        u8_t physical_block_exp;
        u8_t alignment_offset;
        u16_t min_io_size;
        u32_t opt_io_size;
    } topology;
} __attribute__((packed));

/* Request header */
struct virtio_blk_outhdr {
    u32_t type;                 /* Request type */
    u32_t reserved;             /* Reserved */
    u64_t sector;               /* Start sector */
} __attribute__((packed));

/* Request types */
#define VIRTIO_BLK_T_IN         0   /* Read */
#define VIRTIO_BLK_T_OUT        1   /* Write */
#define VIRTIO_BLK_T_FLUSH      4   /* Flush */
#define VIRTIO_BLK_T_GET_ID     8   /* Get device ID */

/* Status codes */
#define VIRTIO_BLK_S_OK         0   /* Success */
#define VIRTIO_BLK_S_IOERR      1   /* I/O error */
#define VIRTIO_BLK_S_UNSUPP     2   /* Unsupported */

#endif /* _VIRTIO_BLK_H */
