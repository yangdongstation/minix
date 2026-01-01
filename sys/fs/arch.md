# MINIX File System Interface System

## Overview

The `sys/fs/` directory contains the file system interface system for MINIX 3, providing essential definitions, utilities, and interfaces for diverse file system implementations. This directory serves as the central hub for file system development, offering standardized interfaces for CD-ROM, MS-DOS, UDF, V7, and other file system types while maintaining compatibility with MINIX's microkernel architecture and message-passing paradigm.

## Purpose and Responsibilities

### Core Functions
- **File System Interface Standardization**: Provides standardized interfaces for multiple file system types
- **Unicode Support**: Offers comprehensive Unicode handling for international file systems
- **Cross-Platform Compatibility**: Ensures file system compatibility across different platforms
- **File System Utilities**: Provides common utilities shared across file system implementations
- **Data Structure Definitions**: Defines standard file system data structures and formats

### Key Features
- **Multiple File System Support** - CD9660, MS-DOS, UDF, V7, and PUFFS file systems
- **Unicode Processing** - Full Unicode support for international file names
- **File System Independence** - Platform-agnostic file system interfaces
- **Standard Compliance** - POSIX and industry standard compliance
- **Performance Optimization** - Efficient file system operations and caching

## Directory Structure

```
sys/fs/
├── Makefile               # Main file system makefile
├── unicode.h             # Unicode processing definitions
├── cd9660/               # ISO 9660 CD-ROM file system
│   ├── cd9660.h          # CD9660 header
│   ├── cd9660_mount.h    # CD9660 mount definitions
│   ├── cd9660_node.h     # CD9660 node structures
│   ├── cd9660.h          # CD9660 main header
│   ├── iso.h             # ISO 9660 definitions
│   └── Makefile          # CD9660 makefile
├── msdosfs/              # MS-DOS FAT file system
│   ├── bpb.h             # BIOS Parameter Block
│   ├── de.h              # Directory entry structures
│   ├── dirent.h          # Directory entry definitions
│   ├── fat.h             # File Allocation Table
│   ├── msdosfs.h         # MS-DOS file system header
│   ├── msdosfs_mount.h   # MS-DOS mount definitions
│   └── Makefile          # MS-DOS makefile
├── puffs/                # Pass-to-Userspace Framework File System
│   ├── puffs.h           # PUFFS header
│   ├── puffs_msgif.h     # PUFFS message interface
│   ├── puffs_msgif.c     # PUFFS message implementation
│   ├── puffs_node.h      # PUFFS node structures
│   ├── puffs_rump.c      # PUFFS RUMP integration
│   ├── puffs_sys.h       # PUFFS system interface
│   └── Makefile          # PUFFS makefile
├── udf/                  # Universal Disk Format
│   ├── ecma167-udf.h     # ECMA-167 UDF definitions
│   ├── udf.h             # UDF main header
│   ├── udf_mount.h       # UDF mount definitions
│   └── Makefile          # UDF makefile
└── v7fs/                 # Version 7 File System
    ├── v7fs.h            # V7 file system header
    ├── v7fs_mount.h      # V7 mount definitions
    └── Makefile          # V7 makefile
```

## Unicode Processing System

### Unicode Character Support
Comprehensive Unicode handling for international file systems:

```c
/* Unicode processing definitions (unicode.h) */

/* UTF-8 encoding/decoding */
int utf8_to_utf32(const char *utf8, size_t len, uint32_t *utf32);
int utf32_to_utf8(uint32_t utf32, char *utf8, size_t len);
size_t utf8_length(const char *utf8, size_t len);
bool utf8_valid(const char *utf8, size_t len);

/* UTF-16 encoding/decoding */
int utf16_to_utf32(const uint16_t *utf16, size_t len, uint32_t *utf32);
int utf32_to_utf16(uint32_t utf32, uint16_t *utf16, size_t len);
size_t utf16_length(const uint16_t *utf16, size_t len);

/* Unicode normalization */
int unicode_normalize_nfc(const uint32_t *src, size_t srclen, uint32_t *dst, size_t dstlen);
int unicode_normalize_nfd(const uint32_t *src, size_t srclen, uint32_t *dst, size_t dstlen);
int unicode_normalize_nfkc(const uint32_t *src, size_t srclen, uint32_t *dst, size_t dstlen);
int unicode_normalize_nfkd(const uint32_t *src, size_t srclen, uint32_t *dst, size_t dstlen);

/* Case conversion */
int unicode_toupper(uint32_t ch);
int unicode_tolower(uint32_t ch);
int unicode_totitle(uint32_t ch);
bool unicode_isupper(uint32_t ch);
bool unicode_islower(uint32_t ch);

/* Character categories */
bool unicode_isalpha(uint32_t ch);
bool unicode_isdigit(uint32_t ch);
bool unicode_isalnum(uint32_t ch);
bool unicode_isspace(uint32_t ch);
bool unicode_ispunct(uint32_t ch);
bool unicode_isprint(uint32_t ch);
bool unicode_isgraph(uint32_t ch);
bool unicode_iscntrl(uint32_t ch);
```

### File System Unicode Integration
Integration of Unicode support with file system operations:

```c
/* File system Unicode conversion */
struct fs_unicode_conv {
    const char *charset;        /* Target character set */
    int (*to_unicode)(const char *src, size_t srclen, uint32_t *dst, size_t dstlen);
    int (*from_unicode)(const uint32_t *src, size_t srclen, char *dst, size_t dstlen);
};

/* Common file system Unicode functions */
int fs_unicode_normalize_filename(const char *src, char *dst, size_t dstlen, int form);
int fs_unicode_casefold_filename(const char *src, char *dst, size_t dstlen);
bool fs_unicode_filename_equal(const char *a, const char *b);
int fs_unicode_to_utf8(const uint32_t *unicode, size_t len, char *utf8, size_t buflen);
```

## CD9660 File System

### ISO 9660 Structure Definitions
CD-ROM file system structures and constants:

```c
/* ISO 9660 definitions (iso.h) */
#define ISO_DEFAULT_BLOCK_SIZE  2048    /* Default block size */
#define ISO_VOLUME_DESCRIPTOR   0       /* Volume descriptor type */
#define ISO_BOOT_DESCRIPTOR     1       /* Boot descriptor type */
#define ISO_TERM_DESCRIPTOR     255     /* Terminator descriptor type */

/* Volume descriptor structure */
struct iso_volume_descriptor {
    uint8_t     type;                   /* Descriptor type */
    uint8_t     id[5];                  /* Standard identifier */
    uint8_t     version;                /* Version */
    uint8_t     flags;                  /* Flags */
    uint8_t     system_id[32];          /* System identifier */
    uint8_t     volume_id[32];          /* Volume identifier */
    uint8_t     unused1[8];             /* Unused */
    uint32_t    volume_space_size;      /* Volume space size */
    uint8_t     unused2[32];            /* Unused */
    uint16_t    volume_set_size;        /* Volume set size */
    uint16_t    volume_sequence_number; /* Volume sequence number */
    uint16_t    logical_block_size;     /* Logical block size */
    uint32_t    path_table_size;        /* Path table size */
    uint32_t    type_l_path_table;      /* Type L path table */
    uint32_t    opt_type_l_path_table;  /* Optional type L path table */
    uint32_t    type_m_path_table;      /* Type M path table */
    uint32_t    opt_type_m_path_table;  /* Optional type M path table */
    uint8_t     root_directory_record[34]; /* Root directory record */
    uint8_t     volume_set_id[128];     /* Volume set identifier */
    uint8_t     publisher_id[128];      /* Publisher identifier */
    uint8_t     data_preparer_id[128];  /* Data preparer identifier */
    uint8_t     application_id[128];    /* Application identifier */
    uint8_t     copyright_file_id[37];  /* Copyright file identifier */
    uint8_t     abstract_file_id[37];   /* Abstract file identifier */
    uint8_t     bibliographic_file_id[37]; /* Bibliographic file identifier */
    uint8_t     creation_date[17];      /* Creation date */
    uint8_t     modification_date[17];  /* Modification date */
    uint8_t     expiration_date[17];    /* Expiration date */
    uint8_t     effective_date[17];     /* Effective date */
    uint8_t     file_structure_version; /* File structure version */
    uint8_t     unused3[1];             /* Unused */
    uint8_t     application_data[512];  /* Application data */
    uint8_t     unused4[653];           /* Unused */
};

/* Directory record structure */
struct iso_directory_record {
    uint8_t     length;                 /* Record length */
    uint8_t     ext_attr_length;        /* Extended attribute length */
    uint32_t    extent;                 /* Extent location */
    uint32_t    size;                   /* Data size */
    uint8_t     date[7];                /* Recording date */
    uint8_t     flags;                  /* File flags */
    uint8_t     file_unit_size;         /* File unit size */
    uint8_t     interleave;             /* Interleave */
    uint16_t    volume_sequence_number; /* Volume sequence number */
    uint8_t     name_len;               /* Name length */
    uint8_t     name[1];                /* Name (variable length) */
};
```

### CD9660 Mount Parameters
Mount-specific parameters for CD9660 file system:

```c
/* CD9660 mount parameters (cd9660_mount.h) */
struct iso_args {
    char *fspec;                /* Block device name */
    int flags;                  /* Mount flags */
    int sector_size;            /* Sector size */
    int session_offset;         /* Session offset */
    uid_t uid;                  /* Override uid */
    gid_t gid;                  /* Override gid */
    mode_t mask;                /* Override mask */
};

/* CD9660 mount flags */
#define ISOFSMNT_NORRIP   0x00000001  /* Disable Rock Ridge */
#define ISOFSMNT_GENS     0x00000002  /* Enable generation numbers */
#define ISOFSMNT_EXTATT   0x00000004  /* Enable extended attributes */
#define ISOFSMNT_NOJOLIET 0x00000008  /* Disable Joliet */
#define ISOFSMNT_SESS     0x00000010  /* Multi-session */
#define ISOFSMNT_RRCASE   0x00000020  /* Rock Ridge case sensitive */
```

## MS-DOS File System

### FAT Structure Definitions
File Allocation Table file system structures:

```c
/* BIOS Parameter Block (bpb.h) */
struct bpb {
    uint8_t     bpb_jmpboot[3];     /* Jump instruction */
    uint8_t     bpb_oemname[8];     /* OEM name */
    uint16_t    bpb_bytpersec;      /* Bytes per sector */
    uint8_t     bpb_secperclus;     /* Sectors per cluster */
    uint16_t    bpb_rsvdseccnt;     /* Reserved sector count */
    uint8_t     bpb_numfats;        /* Number of FATs */
    uint16_t    bpb_rootentcnt;     /* Root entry count */
    uint16_t    bpb_totsec16;       /* Total sectors 16-bit */
    uint8_t     bpb_media;          /* Media descriptor */
    uint16_t    bpb_fatsz16;        /* FAT size 16-bit */
    uint16_t    bpb_secpertrk;      /* Sectors per track */
    uint16_t    bpb_numheads;       /* Number of heads */
    uint32_t    bpb_hiddsec;        /* Hidden sectors */
    uint32_t    bpb_totsec32;       /* Total sectors 32-bit */
    
    /* Extended BPB for FAT32 */
    uint32_t    bpb_fatsz32;        /* FAT size 32-bit */
    uint16_t    bpb_extflags;       /* Extended flags */
    uint16_t    bpb_fsver;          /* File system version */
    uint32_t    bpb_rootclus;       /* Root cluster */
    uint16_t    bpb_fsinfo;         /* FSInfo sector */
    uint16_t    bpb_bkbootsec;      /* Backup boot sector */
    uint8_t     bpb_reserved[12];   /* Reserved */
    uint8_t     bpb_drvnum;         /* Drive number */
    uint8_t     bpb_reserved1;      /* Reserved */
    uint8_t     bpb_bootsig;        /* Boot signature */
    uint32_t    bpb_volid;          /* Volume ID */
    uint8_t     bpb_vollab[11];     /* Volume label */
    uint8_t     bpb_fil systype[8]; /* File system type */
};

/* Directory entry structure (de.h) */
struct direntry {
    uint8_t     deName[11];         /* File name */
    uint8_t     deAttributes;       /* File attributes */
    uint8_t     deLowerCase;        /* Lower case */
    uint8_t     deCHundredth;       /* Creation time (1/100s) */
    uint16_t    deCTime;            /* Creation time */
    uint16_t    deCDate;            /* Creation date */
    uint16_t    deADate;            /* Access date */
    uint16_t    deHighClust;        /* High cluster (FAT32) */
    uint16_t    deMTime;            /* Modification time */
    uint16_t    deMDate;            /* Modification date */
    uint16_t    deStartCluster;     /* Start cluster */
    uint32_t    deFileSize;         /* File size */
};

/* File attributes */
#define ATTR_READONLY   0x01    /* Read only */
#define ATTR_HIDDEN     0x02    /* Hidden */
#define ATTR_SYSTEM     0x04    /* System */
#define ATTR_VOLUME_ID  0x08    /* Volume ID */
#define ATTR_DIRECTORY  0x10    /* Directory */
#define ATTR_ARCHIVE    0x20    /* Archive */
#define ATTR_LONG_NAME  0x0F    /* Long name */
```

### FAT Management
File Allocation Table management structures:

```c
/* FAT types */
typedef enum { FAT12, FAT16, FAT32 } fattype;

/* FAT information structure (fat.h) */
struct fat_info {
    fattype     fat_type;           /* FAT type */
    uint32_t    fat_size;           /* FAT size in sectors */
    uint32_t    cluster_count;      /* Number of clusters */
    uint32_t    data_sector;        /* First data sector */
    uint32_t    root_sector;        /* Root directory sector */
    uint32_t    root_entries;       /* Root directory entries */
    uint32_t    sectors_per_cluster;/* Sectors per cluster */
    uint32_t    bytes_per_sector;   /* Bytes per sector */
    uint32_t    reserved_sectors;   /* Reserved sectors */
    uint32_t    fat_sectors;        /* FAT sectors */
    uint32_t    total_sectors;      /* Total sectors */
    uint32_t    bad_clusters;       /* Bad clusters */
    uint32_t    free_clusters;      /* Free clusters */
    uint32_t    next_cluster;       /* Next free cluster */
};

/* Cluster allocation */
uint32_t fat_get_next_cluster(struct fat_info *fat, uint32_t cluster);
int fat_set_next_cluster(struct fat_info *fat, uint32_t cluster, uint32_t next);
uint32_t fat_alloc_cluster(struct fat_info *fat);
int fat_free_cluster_chain(struct fat_info *fat, uint32_t start_cluster);
```

## PUFFS File System

### Pass-to-Userspace Framework
Userspace file system interface:

```c
/* PUFFS message interface (puffs_msgif.h) */
struct puffs_mount_info {
    uint32_t    pmi_version;        /* PUFFS version */
    uint32_t    pmi_flags;          /* Mount flags */
    uint32_t    pmi_maxmsg;         /* Maximum message size */
    uint32_t    pmi_maxcache;       /* Maximum cache size */
    uint64_t    pmi_root_cookie;    /* Root node cookie */
    char        pmi_servname[PUFFS_NAMESIZE]; /* Server name */
};

/* PUFFS operation types */
enum puffs_op {
    PUFFS_OP_LOOKUP,        /* Lookup operation */
    PUFFS_OP_CREATE,        /* Create operation */
    PUFFS_OP_OPEN,          /* Open operation */
    PUFFS_OP_CLOSE,         /* Close operation */
    PUFFS_OP_READ,          /* Read operation */
    PUFFS_OP_WRITE,         /* Write operation */
    PUFFS_OP_IOCTL,         /* IOCTL operation */
    PUFFS_OP_POLL,          /* Poll operation */
    PUFFS_OP_FSYNC,         /* FSYNC operation */
    PUFFS_OP_LINK,          /* Link operation */
    PUFFS_OP_UNLINK,        /* Unlink operation */
    PUFFS_OP_RENAME,        /* Rename operation */
    PUFFS_OP_MKDIR,         /* MKDIR operation */
    PUFFS_OP_RMDIR,         /* RMDIR operation */
    PUFFS_OP_GETATTR,       /* GETATTR operation */
    PUFFS_OP_SETATTR,       /* SETATTR operation */
    PUFFS_OP_INACTIVE,      /* INACTIVE operation */
    PUFFS_OP_RECLAIM,       /* RECLAIM operation */
    PUFFS_OP_STRATEGY,      /* STRATEGY operation */
    PUFFS_OP_BMAP,          /* BMAP operation */
    PUFFS_OP_PRINT,         /* PRINT operation */
    PUFFS_OP_READLINK,      /* READLINK operation */
    PUFFS_OP_SYMLINK,       /* SYMLINK operation */
    PUFFS_OP_MKNOD,         /* MKNOD operation */
    PUFFS_OP_ACCESS,        /* ACCESS operation */
    PUFFS_OP_ADVLOCK,       /* ADVLOCK operation */
    PUFFS_OP_ALLOC,         /* ALLOC operation */
    PUFFS_OP_BALLOC,        /* BALLOC operation */
    PUFFS_OP_BWRITE,        /* BWRITE operation */
    PUFFS_OP_CMP,           /* CMP operation */
    PUFFS_OP_CONVERT,       /* CONVERT operation */
    PUFFS_OP_COPY,          /* COPY operation */
    PUFFS_OP_COPY_FILE_RANGE, /* COPY_FILE_RANGE operation */
    PUFFS_OP_FALLOCATE,     /* FALLOCATE operation */
    PUFFS_OP_FCNTL,         /* FCNTL operation */
    PUFFS_OP_FLOCK,         /* FLOCK operation */
    PUFFS_OP_GETPAGES,      /* GETPAGES operation */
    PUFFS_OP_PUTPAGES,      /* PUTPAGES operation */
    PUFFS_OP_KQFILTER,      /* KQFILTER operation */
    PUFFS_OP_MMAP,          /* MMAP operation */
    PUFFS_OP_PATHCONF,      /* PATHCONF operation */
    PUFFS_OP_READV,         /* READV operation */
    PUFFS_OP_WRITEV,        /* WRITEV operation */
    PUFFS_OP_SEEK,          /* SEEK operation */
    PUFFS_OP_REMOVE,        /* REMOVE operation */
    PUFFS_OP_RMDIR,         /* RMDIR operation */
    PUFFS_OP_SETACL,        /* SETACL operation */
    PUFFS_OP_GETACL,        /* GETACL operation */
    PUFFS_OP_SETEXTATTR,    /* SETEXTATTR operation */
    PUFFS_OP_GETEXTATTR,    /* GETEXTATTR operation */
    PUFFS_OP_LSEEK,         /* LSEEK operation */
    PUFFS_OP_TRUNCATE,      /* TRUNCATE operation */
    PUFFS_OP_UPDATE,        /* UPDATE operation */
    PUFFS_OP_WAPBL_BEGIN,   /* WAPBL_BEGIN operation */
    PUFFS_OP_WAPBL_END,     /* WAPBL_END operation */
};
```

## UDF File System

### Universal Disk Format
DVD and optical media file system support:

```c
/* UDF definitions (udf.h) */
#define UDF_BLOCK_SIZE      2048    /* UDF block size */
#define UDF_VRS_OFFSET      32768   /* Volume recognition sequence offset */

/* UDF tag structure */
struct udf_tag {
    uint16_t    tag_ident;          /* Tag identifier */
    uint16_t    tag_descriptor_version; /* Descriptor version */
    uint8_t     tag_checksum;       /* Tag checksum */
    uint8_t     tag_reserved;       /* Reserved */
    uint16_t    tag_serial_number;  /* Tag serial number */
    uint16_t    tag_crc;            /* Tag CRC */
    uint32_t    tag_location;       /* Tag location */
};

/* UDF primary volume descriptor */
struct udf_primary_volume_descriptor {
    struct udf_tag pvd_tag;         /* Descriptor tag */
    uint32_t    pvd_volume_sequence; /* Volume sequence number */
    uint32_t    pvd_primary_volume; /* Primary volume number */
    uint8_t     pvd_volume_id[32];  /* Volume identifier */
    uint16_t    pvd_volume_set_size; /* Volume set size */
    uint16_t    pvd_volume_sequence_number; /* Volume sequence number */
    uint32_t    pvd_logical_block_size; /* Logical block size */
    uint32_t    pvd_domain_id[4];   /* Domain identifier */
    uint8_t     pvd_volume_contents[16]; /* Volume contents */
    uint32_t    pvd_map_table_length; /* Map table length */
    uint32_t    pvd_partition_maps; /* Number of partition maps */
    uint8_t     pvd_implementation_id[32]; /* Implementation identifier */
    uint8_t     pvd_volume_contents_use[128]; /* Volume contents use */
};
```

## V7 File System

### Version 7 File System
Traditional UNIX Version 7 file system support:

```c
/* V7 file system definitions (v7fs.h) */
#define V7FS_BSIZE      512     /* V7 block size */
#define V7FS_BSHIFT     9       /* V7 block shift */
#define V7FS_NDADDR     8       /* Direct addresses */
#define V7FS_NIADDR     3       /* Indirect addresses */

/* V7 superblock structure */
struct v7fs_superblock {
    int16_t     s_isize;            /* Size of i-list in blocks */
    int32_t     s_fsize;            /* Size of file system in blocks */
    int32_t     s_nfree;            /* Number of free blocks */
    int32_t     s_free[100];        /* Free block list */
    int16_t     s_ninode;           /* Number of free inodes */
    int16_t     s_inode[100];       /* Free inode list */
    uint8_t     s_flock;            /* Free block lock */
    uint8_t     s_ilock;            /* Free inode lock */
    uint8_t     s_fmod;             /* Superblock modified */
    uint8_t     s_ronly;            /* Read-only flag */
    int32_t     s_time[2];          /* Time */
    int16_t     s_dinfo[4];         /* Device information */
};

/* V7 inode structure */
struct v7fs_inode {
    uint16_t    i_mode;             /* File mode */
    uint16_t    i_nlink;            /* Link count */
    uint16_t    i_uid;              /* User ID */
    uint16_t    i_gid;              /* Group ID */
    uint8_t     i_size0;            /* Size high byte */
    uint16_t    i_size1;            /* Size low bytes */
    int32_t     i_addr[V7FS_NDADDR]; /* Direct block addresses */
    int32_t     i_atime[2];         /* Access time */
    int32_t     i_mtime[2];         /* Modification time */
};
```

## Integration with MINIX VFS

### VFS Message Interface
Integration with MINIX Virtual File System server:

```c
/* File system message interface */
struct fs_message {
    int     fs_type;            /* Message type */
    int     fs_operation;       /* File system operation */
    int     fs_result;          /* Operation result */
    
    union {
        /* Read/write operations */
        struct {
            int fd;             /* File descriptor */
            void *buf;          /* Buffer */
            size_t count;       /* Byte count */
            off_t offset;       /* File offset */
        } rw;
        
        /* Open/close operations */
        struct {
            const char *path;   /* File path */
            int flags;          /* Open flags */
            mode_t mode;        /* Creation mode */
        } open;
        
        /* Stat operations */
        struct {
            const char *path;   /* File path */
            struct stat *sb;    /* Stat buffer */
        } stat;
        
        /* Directory operations */
        struct {
            const char *path;   /* Directory path */
            struct dirent *ent; /* Directory entry */
        } dir;
    } u;
};

/* File system operation handlers */
int fs_handle_read(struct fs_message *msg);
int fs_handle_write(struct fs_message *msg);
int fs_handle_open(struct fs_message *msg);
int fs_handle_close(struct fs_message *msg);
int fs_handle_stat(struct fs_message *msg);
int fs_handle_mkdir(struct fs_message *msg);
int fs_handle_unlink(struct fs_message *msg);
```

### File System Registration
File system registration with VFS:

```c
/* File system registration */
struct fs_type {
    const char *name;           /* File system name */
    int (*mount)(struct mount *mp, const char *path, int flags, void *data);
    int (*unmount)(struct mount *mp, int flags);
    int (*root)(struct mount *mp, struct vnode **vpp);
    int (*statvfs)(struct mount *mp, struct statvfs *sbp);
    int (*sync)(struct mount *mp, int waitfor);
    void (*init)(void);
};

/* File system registration functions */
int vfs_register_fs(const char *name, const struct fs_type *fst);
int vfs_unregister_fs(const char *name);
const struct fs_type *vfs_find_fs(const char *name);
```

## Build System Integration

### File System Build Configuration
Integration with MINIX build system:

```makefile
# sys/fs/Makefile
# File system interface build configuration

# Core file system interfaces
INCLUDES += -I${.CURDIR}

# CD9660 file system
SUBDIR += cd9660

# MS-DOS file system
SUBDIR += msdosfs

# PUFFS file system
SUBDIR += puffs

# UDF file system
SUBDIR += udf

# V7 file system
SUBDIR += v7fs

# Unicode support
HDRS += unicode.h

# Installation
includes:
	install -m 444 unicode.h ${DESTDIR}/usr/include/fs/
```

### Cross-Platform File System Support
Platform-agnostic file system interfaces:

```makefile
# Architecture-independent file systems
ARCH_INDEPENDENT_FS = cd9660 msdosfs udf v7fs

# Architecture-specific optimizations
.if ${MACHINE_ARCH} == "evbriscv64"
CFLAGS += -DRISCV64_OPTIMIZED
.elif ${MACHINE_ARCH} == "i386"
CFLAGS += -DI386_OPTIMIZED
.elif ${MACHINE_ARCH} == "arm"
CFLAGS += -DARM_OPTIMIZED
.endif
```

## Testing and Validation

### File System Test Framework
Comprehensive testing for file system interfaces:

```c
/* File system test framework */
struct fs_test_case {
    const char *name;               /* Test name */
    const char *fs_type;            /* File system type */
    int (*test_func)(void);         /* Test function */
    int expected_result;            /* Expected result */
};

/* Unicode testing */
static struct fs_test_case unicode_tests[] = {
    { "utf8_to_utf32", "unicode", test_utf8_to_utf32, 0 },
    { "utf32_to_utf8", "unicode", test_utf32_to_utf8, 0 },
    { "unicode_normalize", "unicode", test_unicode_normalize, 0 },
    { "unicode_casefold", "unicode", test_unicode_casefold, 0 },
    /* ... additional tests ... */
};

/* File system operation testing */
static struct fs_test_case fs_op_tests[] = {
    { "cd9660_read", "cd9660", test_cd9660_read, 0 },
    { "msdosfs_write", "msdosfs", test_msdosfs_write, 0 },
    { "udf_mount", "udf", test_udf_mount, 0 },
    { "v7fs_create", "v7fs", test_v7fs_create, 0 },
    /* ... additional tests ... */
};
```

### Cross-File System Compatibility
Ensures compatibility across different file system types:

```c
/* Cross-file system compatibility tests */
int test_cross_fs_compatibility(void) {
    int result = 0;
    
    /* Test Unicode handling across file systems */
    result |= test_unicode_compatibility();
    
    /* Test file name handling */
    result |= test_filename_compatibility();
    
    /* Test attribute handling */
    result |= test_attribute_compatibility();
    
    /* Test permission handling */
    result |= test_permission_compatibility();
    
    return result;
}
```

The file system interface system provides the essential foundation that enables MINIX 3 to support diverse file system types while maintaining compatibility with the microkernel architecture and message-passing paradigm. This comprehensive file system framework ensures consistent and reliable file system operations across different storage media and file system formats.