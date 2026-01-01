# MINIX Database Library Headers Architecture

## File Overview

The `/include/db.h` file contains the Berkeley DB interface definitions, providing a comprehensive embedded database library for MINIX applications. This header defines the API for key-value database operations, supporting both hash table and B-tree access methods for efficient data storage and retrieval.

## Database Architecture Foundation

### Database Models
- **Hash Tables**: O(1) average-case access for exact key matches
- **B-Trees**: O(log n) access with ordered key traversal
- **Record Numbers**: Direct access by record position in B-trees
- **Queue Access**: FIFO queue operations for specialized applications
- **Recno Access**: Record-based access with variable-length records

### Standards Compliance
- **Berkeley DB 1.85**: Original BSD database library interface
- **POSIX.1**: Portable operating system interface compatibility
- **ANSI C**: Standard C library interface conventions
- **Thread Safety**: Reentrant design for multi-threaded applications

## Core Database Interface

### Database Handle Management

#### Database Structure
**Purpose**: Primary database handle structure for all database operations

```c
// Database handle structure
typedef struct {
    void *internal;                 // Internal database state
    int close;                      // Close function pointer
    int del;                        // Delete function pointer
    int get;                        // Get function pointer
    int put;                        // Put function pointer
    int seq;                        // Sequential access function pointer
    int sync;                       // Synchronization function pointer
} DB;

// Database type definitions
typedef struct {
    char *data;                     // Data buffer pointer
    size_t size;                    // Data size
} DBT;

typedef struct {
    DBT key;                        // Key data
    DBT data;                       // Associated data
} DBT_PAIR;
```

**Database Return Codes**:
```c
// Database operation return codes
#define RET_ERROR       -1          // General error condition
#define RET_SUCCESS     0           // Operation succeeded
#define RET_SPECIAL     1           // Special return condition
#define RET_KEYEXIST    2           // Key already exists
#define RET_KEYEMPTY    3           // Key does not exist
```

#### Database Configuration
**Purpose**: Database configuration parameters and access method selection

```c
// Database access methods
#define DB_BTREE        1           // B-tree access method
#define DB_HASH         2           // Hash table access method
#define DB_RECNO        3           // Record number access method
#define DB_QUEUE        4           // Queue access method

// Database open flags
#define O_RDONLY        0x0000      // Open for reading only
#define O_WRONLY        0x0001      // Open for writing only
#define O_RDWR          0x0002      // Open for reading and writing
#define O_CREAT         0x0200      // Create if non-existent
#define O_TRUNC         0x0400      // Truncate on open
#define O_EXCL          0x0800      // Error if already exists

// Database cache and page size configuration
#define DEFAULT_CACHE_SIZE  (64 * 1024)     // Default cache size
#define MIN_PAGE_SIZE       512             // Minimum page size
#define MAX_PAGE_SIZE       65536           // Maximum page size
#define DEFAULT_PAGE_SIZE   4096            // Default page size

// Hash table configuration
#define DEFAULT_HASH_SIZE   131071          // Default hash table size
#define MIN_HASH_SIZE       31              // Minimum hash table size
```

### Database Operations

#### Database Opening and Closing
**Purpose**: Database lifecycle management functions

```c
// Database open function
DB *dbopen(const char *file, int flags, int mode, DBTYPE type, const void *openinfo);

// Database close function
int (close)(DB *db);

// Database synchronization
int (sync)(DB *db, uint32_t flags);
```

**Database Open Information Structure**:
```c
// B-tree specific open information
struct btree_info {
    int flags;                      // B-tree flags
    int cachesize;                  // Cache size
    int maxkeypage;                 // Maximum keys per page
    int minkeypage;                 // Minimum keys per page
    int psize;                      // Page size
    int (*compare)(const DBT *, const DBT *); // Comparison function
    size_t prefix;                  // Prefix length
    int lorder;                     // Byte order
};

// Hash table specific open information
struct hash_info {
    int bsize;                      // Bucket size
    int ffactor;                    // Fill factor
    int nelem;                      // Number of elements
    int cachesize;                  // Cache size
    int32_t *hash;                  // Hash function
    int lorder;                     // Byte order
};

// Record number specific open information
struct recno_info {
    int flags;                      // Record flags
    int cachesize;                  // Cache size
    int psize;                      // Page size
    int lorder;                     // Byte order
    size_t reclen;                  // Record length
    uint8_t bval;                   // Byte value
    char *bfname;                   // Backup file name
};
```

#### Data Retrieval and Storage
**Purpose**: Core database operations for data access

```c
// Retrieve data by key
int (get)(DB *db, DBT *key, DBT *data, uint32_t flags);

// Store data with key
int (put)(DB *db, DBT *key, DBT *data, uint32_t flags);

// Delete data by key
int (del)(DB *db, DBT *key, uint32_t flags);

// Sequential access
int (seq)(DB *db, DBT *key, DBT *data, uint32_t flags);
```

**Data Retrieval Flags**:
```c
// Get operation flags
#define R_CURSOR      1               // Cursor positioned access
#define R_FIRST       2               // First record
#define R_LAST        3               // Last record
#define R_NEXT        4               // Next record
#define R_PREV        5               // Previous record

// Put operation flags
#define R_NOOVERWRITE 1               // Don't overwrite existing key
#define R_SETCURSOR   2               // Set cursor position

// Delete operation flags
#define R_CURSOR      1               // Delete at cursor position
```

### B-Tree Access Method

#### B-Tree Configuration
**Purpose**: B-tree specific configuration and optimization parameters

```c
// B-tree specific flags
#define R_DUP         0x01            // Allow duplicate keys
#define R_RECNUM      0x02            // Record number access
#define R_FIXEDLEN    0x04            // Fixed length records

// B-tree page management
#define BTREE_MAX_PAGE  65536         // Maximum page size
#define BTREE_MIN_PAGE  512           // Minimum page size
#define BTREE_DEF_PAGE  4096          // Default page size

// B-tree performance tuning
#define BTREE_DEF_MINKEY  2           // Default minimum keys
#define BTREE_DEF_MAXKEY  0           // Default maximum keys (unlimited)
```

#### B-Tree Comparison Functions
**Purpose**: Custom key comparison for B-tree ordering

```c
// Standard comparison functions
int numeric_compare(const DBT *a, const DBT *b);
int string_compare(const DBT *a, const DBT *b);
int case_compare(const DBT *a, const DBT *b);

// Custom comparison function prototype
typedef int (*cmp_func_t)(const DBT *, const DBT *);
```

### Hash Table Access Method

#### Hash Table Configuration
**Purpose**: Hash table specific configuration and performance parameters

```c
// Hash table performance parameters
#define HASH_DEF_BSIZE  4096          // Default bucket size
#define HASH_DEF_FFACTOR 8            // Default fill factor
#define HASH_DEF_NELEM  1             // Default number of elements

// Hash table limits
#define HASH_MIN_BSIZE  512           // Minimum bucket size
#define HASH_MAX_BSIZE  65536         // Maximum bucket size
#define HASH_MIN_FFACTOR 1            // Minimum fill factor
#define HASH_MAX_FFACTOR 256          // Maximum fill factor
```

#### Hash Functions
**Purpose**: Pluggable hash functions for key distribution

```c
// Default hash functions
uint32_t default_hash(const void *key, size_t len);
uint32_t jenkin_hash(const void *key, size_t len);
uint32_t murmur_hash(const void *key, size_t len);

// Custom hash function prototype
typedef uint32_t (*hash_func_t)(const void *, size_t);
```

### Record Number Access Method

#### Record Number Configuration
**Purpose**: Fixed and variable-length record access configuration

```c
// Record number specific flags
#define R_FIXEDLEN    0x04            // Fixed length records
#define R_NOKEY       0x08            // No explicit keys
#define R_SNAPSHOT    0x10            // Snapshot isolation

// Record delimiter and padding
#define RECNO_DEF_DELIMITER '\n'      // Default record delimiter
#define RECNO_DEF_PAD       ' '       // Default record padding
```

## Advanced Database Features

### Transaction Support

#### Transaction Management
**Purpose**: ACID transaction properties for database consistency

```c
// Transaction state
typedef enum {
    TXN_ABORTED,                    // Transaction aborted
    TXN_COMMITTED,                  // Transaction committed
    TXN_PREPARED,                   // Transaction prepared
    TXN_RUNNING                     // Transaction running
} txn_state_t;

// Transaction control
int txn_begin(DB *db, TXN **txn);
int txn_commit(TXN *txn, uint32_t flags);
int txn_abort(TXN *txn);
```

#### Concurrency Control
**Purpose**: Multi-user database access with consistency guarantees

```c
// Lock modes
typedef enum {
    DB_LOCK_READ,                   // Shared read lock
    DB_LOCK_WRITE,                  // Exclusive write lock
    DB_LOCK_IWRITE,                 // Intent to write
    DB_LOCK_IREAD,                  // Intent to read
    DB_LOCK_IWR                     // Intent to write/read
} db_lock_mode_t;

// Deadlock detection
#define DB_LOCK_NOWAIT    0x01      // Don't wait for locks
#define DB_LOCK_WAIT      0x02      // Wait for locks
#define DB_LOCK_TIMEOUT   0x04      // Timeout for lock acquisition
```

### Database Caching

#### Cache Management
**Purpose**: Memory management and I/O optimization

```c
// Cache configuration
struct cache_config {
    size_t cache_size;              // Cache size in bytes
    int cache_count;                // Number of cache pages
    int priority;                   // Cache priority
    uint32_t flags;                 // Cache flags
};

// Cache statistics
struct cache_stats {
    uint32_t hits;                  // Cache hits
    uint32_t misses;                // Cache misses
    uint32_t writes;                // Cache writes
    uint32_t reads;                 // Cache reads
};
```

### Database Recovery

#### Recovery Mechanisms
**Purpose**: Crash recovery and database consistency restoration

```c
// Recovery procedures
int db_recover(DB *db, const char *file, uint32_t flags);
int db_verify(DB *db, const char *file);
int db_archive(DB *db, char **file_list);

// Recovery flags
#define DB_RECOVER_FATAL  0x01      // Fatal error recovery
#define DB_RECOVER_ERR    0x02      // Error recovery
#define DB_VERIFY_FATAL   0x04      // Fatal verification
```

## RISC-V 64-bit Architecture Support

### Memory Alignment Requirements
```c
// Aligned structures for RISC-V 64-bit
struct DBT {
    char *data;                     // 8-byte pointer on RV64
    size_t size;                    // 8-byte on RV64 (aligned)
} __attribute__((aligned(8)));

struct DB {
    void *internal;                 // 8-byte pointer on RV64
    int (*close)(DB *);             // Function pointer (8 bytes)
    int (*del)(DB *, DBT *, uint32_t);
    int (*get)(DB *, DBT *, DBT *, uint32_t);
    int (*put)(DB *, DBT *, DBT *, uint32_t);
    int (*seq)(DB *, DBT *, DBT *, uint32_t);
    int (*sync)(DB *, uint32_t);
} __attribute__((aligned(8)));
```

### Performance Optimizations
- **64-bit Arithmetic**: Efficient key comparison and hash calculations
- **Memory Access**: Aligned database page access
- **Large File Support**: Native 64-bit file offsets
- **Cache Efficiency**: Database page size aligned to cache lines

### Atomic Operations
```c
// Atomic database statistics updates
static __inline void dbstat_inc(uint32_t *stat) {
    __atomic_fetch_add(stat, 1, __ATOMIC_RELAXED);
}

// Atomic page reference counting
static __inline void pageref_inc(uint32_t *refcnt) {
    __atomic_fetch_add(refcnt, 1, __ATOMIC_SEQ_CST);
}

static __inline void pageref_dec(uint32_t *refcnt) {
    __atomic_fetch_sub(refcnt, 1, __ATOMIC_SEQ_CST);
}
```

## MINIX Microkernel Integration

### Message-Based Database Operations
MINIX database operations integrate with the message-passing architecture:

```c
// Database operation through VFS server
message m;
m.m_type = VFS_DB_OPEN;
m.VFS_DB_FILE = filename;
m.VFS_DB_FLAGS = flags;
m.VFS_DB_MODE = mode;
m.VFS_DB_TYPE = DBTYPE;
_sendrec(VFS_PROC_NR, &m);

// Database get operation
m.m_type = VFS_DB_GET;
m.VFS_DB_HANDLE = db_handle;
m.VFS_DB_KEY = key_buffer;
m.VFS_DB_KEYLEN = key_length;
m.VFS_DB_DATA = data_buffer;
m.VFS_DB_DATALEN = data_length;
_sendrec(VFS_PROC_NR, &m);
```

### Userspace Database Server
- **Database Manager**: Userspace server managing database operations
- **Cache Management**: Coordinated cache with VM system
- **Transaction Coordination**: Multi-server transaction support
- **Recovery Service**: Coordinated crash recovery

### Device Driver Integration
```c
// Block device database storage
struct db_block_device {
    dev_t device;                   // Block device number
    uint64_t offset;                // Database offset on device
    uint32_t block_size;            // Block size
    uint32_t cache_size;            // Cache size
};

// Raw device database access
int db_raw_read(dev_t device, uint64_t offset, void *buffer, size_t size);
int db_raw_write(dev_t device, uint64_t offset, const void *buffer, size_t size);
```

## Security Framework

### Database Access Control
```c
// Database access permissions
struct db_acl {
    char *principal;                // User/group principal
    uint32_t permissions;           // Permission bits
    uint32_t flags;                 // Special flags
};

// Permission bits
#define DB_PERM_READ    0x01        // Read permission
#define DB_PERM_WRITE   0x02        // Write permission
#define DB_PERM_CREATE  0x04        // Create permission
#define DB_PERM_DELETE  0x08        // Delete permission
#define DB_PERM_ADMIN   0x10        // Administrative permission
```

### Encryption Support
```c
// Database encryption
struct db_crypto {
    uint32_t algorithm;             // Encryption algorithm
    uint8_t key[32];                // Encryption key
    uint8_t iv[16];                 // Initialization vector
    uint32_t flags;                 // Encryption flags
};

// Encryption algorithms
#define DB_CRYPTO_AES128    1       // AES-128 encryption
#define DB_CRYPTO_AES256    2       // AES-256 encryption
#define DB_CRYPTO_3DES      3       // 3DES encryption
```

### Audit and Logging
```c
// Database audit trail
struct db_audit {
    uint32_t operation;             // Database operation
    DBT key;                        // Operation key
    DBT data;                       // Operation data
    time_t timestamp;               // Operation timestamp
    uid_t user;                     // User performing operation
};

// Audit operations
#define DB_AUDIT_GET        1       // Get operation
#define DB_AUDIT_PUT        2       // Put operation
#define DB_AUDIT_DEL        3       // Delete operation
#define DB_AUDIT_COMMIT     4       // Transaction commit
#define DB_AUDIT_ABORT      5       // Transaction abort
```

## Development Guidelines

### Database Application Pattern
```c
// Complete database application example
#include <db.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    DB *db;
    DBT key, data;
    int result;
    
    // Open database
    db = dbopen("mydb.db", O_RDWR | O_CREAT, 0644, DB_BTREE, NULL);
    if (db == NULL) {
        perror("dbopen");
        return 1;
    }
    
    // Prepare key and data
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));
    
    key.data = "test_key";
    key.size = strlen("test_key") + 1;
    data.data = "test_data";
    data.size = strlen("test_data") + 1;
    
    // Store data
    result = db->put(db, &key, &data, 0);
    if (result != RET_SUCCESS) {
        fprintf(stderr, "put failed: %d\n", result);
        db->close(db);
        return 1;
    }
    
    // Retrieve data
    result = db->get(db, &key, &data, 0);
    if (result == RET_SUCCESS) {
        printf("Retrieved: %s\n", (char *)data.data);
    }
    
    // Close database
    db->close(db);
    return 0;
}
```

### B-Tree Development Pattern
```c
// Custom B-tree comparison function
int my_compare(const DBT *a, const DBT *b) {
    // Custom comparison logic
    size_t min_len = (a->size < b->size) ? a->size : b->size;
    int result = memcmp(a->data, b->data, min_len);
    
    if (result != 0)
        return result;
    
    if (a->size < b->size)
        return -1;
    if (a->size > b->size)
        return 1;
    
    return 0;
}

// B-tree with custom comparison
struct btree_info btinfo = {
    .flags = 0,
    .cachesize = 64 * 1024,
    .psize = 4096,
    .compare = my_compare,
    .lorder = 0
};

DB *db = dbopen("btree.db", O_RDWR | O_CREAT, 0644, DB_BTREE, &btinfo);
```

### Error Handling Pattern
```c
// Comprehensive database error handling
const char *db_err_string(int error) {
    switch (error) {
    case RET_SUCCESS:
        return "Success";
    case RET_ERROR:
        return "General error";
    case RET_KEYEXIST:
        return "Key already exists";
    case RET_KEYEMPTY:
        return "Key does not exist";
    default:
        return "Unknown error";
    }
}

// Error handling in operations
int safe_put(DB *db, DBT *key, DBT *data) {
    int result = db->put(db, key, data, R_NOOVERWRITE);
    
    switch (result) {
    case RET_SUCCESS:
        return 0;
    case RET_KEYEXIST:
        fprintf(stderr, "Key already exists\n");
        return -1;
    case RET_ERROR:
        perror("Database put");
        return -1;
    default:
        fprintf(stderr, "Unexpected error: %d\n", result);
        return -1;
    }
}
```

### Performance Optimization Pattern
```c
// High-performance database operations
struct db_cache {
    void *pages[DB_CACHE_PAGES];
    uint32_t refcnt[DB_CACHE_PAGES];
    uint32_t flags[DB_CACHE_PAGES];
};

// Optimized page access
void *db_get_page(struct db_cache *cache, uint32_t pgno) {
    uint32_t slot = pgno % DB_CACHE_PAGES;
    
    if (cache->pages[slot] && cache->refcnt[slot] > 0) {
        // Cache hit
        __atomic_fetch_add(&cache->refcnt[slot], 1, __ATOMIC_SEQ_CST);
        return cache->pages[slot];
    }
    
    // Cache miss - load from disk
    return db_load_page(pgno);
}
```

## Testing and Validation

### Database Functionality Testing
- **CRUD Operations**: Test create, read, update, delete operations
- **Transaction Integrity**: Validate ACID properties
- **Concurrency**: Test multi-user access patterns
- **Recovery**: Test crash recovery mechanisms

### Performance Testing
- **Throughput**: Measure operations per second
- **Latency**: Measure operation response times
- **Scalability**: Test with large datasets
- **Cache Performance**: Measure cache hit ratios

### Security Testing
- **Access Control**: Validate permission checking
- **Input Validation**: Test malformed input handling
- **Encryption**: Test database encryption features
- **Audit**: Verify audit trail generation

This comprehensive database interface provides a complete embedded database solution for MINIX applications, maintaining compatibility with Berkeley DB standards while optimizing for the RISC-V 64-bit microkernel architecture and providing advanced features for modern database applications.