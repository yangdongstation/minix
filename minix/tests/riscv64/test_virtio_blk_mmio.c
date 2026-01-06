/*
 * VirtIO block MMIO I/O test for RISC-V 64
 *
 * This test exercises readv/writev against a block device or file path.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#if defined(__riscv) || defined(__riscv64)
__asm__(".globl __global_pointer$\n"
    ".set __global_pointer$, _gp\n");
#endif

#define VIRTIO_BLK_BLOCK_SIZE 512
#define DEFAULT_OFFSET (1024 * 1024)
#define DEFAULT_SIZE (4096)

static int test_count;
static int pass_count;
static int fail_count;
static int skip_count;

static void report_result(const char *name, int ok)
{
    test_count++;
    if (ok) {
        pass_count++;
        printf("[PASS] %s\n", name);
    } else {
        fail_count++;
        printf("[FAIL] %s (errno=%d)\n", name, errno);
    }
}

static void report_skip(const char *name, const char *reason)
{
    test_count++;
    skip_count++;
    printf("[SKIP] %s: %s\n", name, reason);
}

static int parse_u64(const char *s, uint64_t *out)
{
    char *end = NULL;
    unsigned long long val;

    errno = 0;
    val = strtoull(s, &end, 0);
    if (errno != 0 || end == s || *end != '\0')
        return -1;

    *out = (uint64_t)val;
    return 0;
}

static void fill_pattern(uint8_t *buf, size_t len, uint32_t seed)
{
    uint32_t v = seed;
    for (size_t i = 0; i < len; i++) {
        v = v * 1103515245u + 12345u;
        buf[i] = (uint8_t)(v >> 16);
    }
}

static int run_io_test(const char *path, off_t offset, size_t size,
    int allow_create, int require_block, int force)
{
    struct stat st;
    int have_stat;
    int is_block = 0;
    int is_regular = 0;
    int fd;
    int open_flags;
    ssize_t r;
    size_t sz1 = VIRTIO_BLK_BLOCK_SIZE;
    size_t sz2 = VIRTIO_BLK_BLOCK_SIZE * 2;
    size_t sz3;
    uint8_t *wbuf1 = NULL;
    uint8_t *wbuf2 = NULL;
    uint8_t *wbuf3 = NULL;
    uint8_t *rbuf1 = NULL;
    uint8_t *rbuf2 = NULL;
    uint8_t *rbuf3 = NULL;
    struct iovec wiov[3];
    struct iovec riov[3];
    int ok = 1;

    have_stat = (stat(path, &st) == 0);
    if (have_stat) {
        if (S_ISBLK(st.st_mode))
            is_block = 1;
        else if (S_ISREG(st.st_mode))
            is_regular = 1;
        else {
            report_skip("target path", "unsupported file type");
            return 0;
        }
    } else if (!allow_create) {
        report_skip("target path", "path does not exist");
        return 0;
    } else {
        is_regular = 1;
    }

    if (require_block && !is_block) {
        report_skip("target path", "not a block device");
        return 0;
    }

    if (is_block && offset < DEFAULT_OFFSET && !force) {
        report_skip("offset safety", "offset below 1MB (use -f to override)");
        return 0;
    }

    if (size % VIRTIO_BLK_BLOCK_SIZE != 0) {
        errno = EINVAL;
        report_result("size alignment", 0);
        return 0;
    }

    if (size < (sz1 + sz2 + VIRTIO_BLK_BLOCK_SIZE)) {
        errno = EINVAL;
        report_result("size minimum", 0);
        return 0;
    }

    sz3 = size - sz1 - sz2;

    open_flags = O_RDWR | O_SYNC;
    if (!have_stat && allow_create)
        open_flags |= O_CREAT;

    fd = open(path, open_flags, 0644);
    report_result("open target", fd >= 0);
    if (fd < 0)
        return 0;

    if (is_regular && allow_create) {
        if (ftruncate(fd, offset + (off_t)size) != 0)
            report_result("ftruncate target", 0);
        else
            report_result("ftruncate target", 1);
    }

    wbuf1 = malloc(sz1);
    wbuf2 = malloc(sz2);
    wbuf3 = malloc(sz3);
    rbuf1 = malloc(sz1);
    rbuf2 = malloc(sz2);
    rbuf3 = malloc(sz3);
    if (!wbuf1 || !wbuf2 || !wbuf3 || !rbuf1 || !rbuf2 || !rbuf3) {
        report_result("allocate buffers", 0);
        close(fd);
        return 0;
    }
    report_result("allocate buffers", 1);

    fill_pattern(wbuf1, sz1, 0x12345678);
    fill_pattern(wbuf2, sz2, 0x87654321);
    fill_pattern(wbuf3, sz3, 0x0f0e0d0c);
    memset(rbuf1, 0, sz1);
    memset(rbuf2, 0, sz2);
    memset(rbuf3, 0, sz3);

    wiov[0].iov_base = wbuf1;
    wiov[0].iov_len = sz1;
    wiov[1].iov_base = wbuf2;
    wiov[1].iov_len = sz2;
    wiov[2].iov_base = wbuf3;
    wiov[2].iov_len = sz3;

    riov[0].iov_base = rbuf1;
    riov[0].iov_len = sz1;
    riov[1].iov_base = rbuf2;
    riov[1].iov_len = sz2;
    riov[2].iov_base = rbuf3;
    riov[2].iov_len = sz3;

    r = lseek(fd, offset, SEEK_SET);
    report_result("seek for writev", r == offset);
    if (r != offset)
        ok = 0;

    errno = 0;
    r = writev(fd, wiov, 3);
    report_result("writev", r == (ssize_t)size);
    if (r != (ssize_t)size)
        ok = 0;

    if (fsync(fd) != 0) {
        report_result("fsync", 0);
        ok = 0;
    } else {
        report_result("fsync", 1);
    }

    r = lseek(fd, offset, SEEK_SET);
    report_result("seek for readv", r == offset);
    if (r != offset)
        ok = 0;

    errno = 0;
    r = readv(fd, riov, 3);
    report_result("readv", r == (ssize_t)size);
    if (r != (ssize_t)size)
        ok = 0;

    if (memcmp(wbuf1, rbuf1, sz1) != 0 ||
        memcmp(wbuf2, rbuf2, sz2) != 0 ||
        memcmp(wbuf3, rbuf3, sz3) != 0) {
        report_result("data verify", 0);
        ok = 0;
    } else {
        report_result("data verify", 1);
    }

    free(wbuf1);
    free(wbuf2);
    free(wbuf3);
    free(rbuf1);
    free(rbuf2);
    free(rbuf3);
    close(fd);

    return ok;
}

static void usage(const char *prog)
{
    printf("Usage: %s -p <path> [-o offset] [-s size] [-c] [-b] [-f]\n", prog);
    printf("  -p  Target path (block device or file)\n");
    printf("  -o  Offset in bytes (default: %d)\n", DEFAULT_OFFSET);
    printf("  -s  Size in bytes (default: %d, must be 512-byte aligned)\n",
        DEFAULT_SIZE);
    printf("  -c  Create file if missing\n");
    printf("  -b  Require block device (skip otherwise)\n");
    printf("  -f  Allow low offsets on block devices\n");
}

int main(int argc, char **argv)
{
    const char *path = NULL;
    uint64_t offset = DEFAULT_OFFSET;
    uint64_t size = DEFAULT_SIZE;
    int allow_create = 0;
    int require_block = 0;
    int force = 0;
    int ch;

    while ((ch = getopt(argc, argv, "p:o:s:cbf")) != -1) {
        switch (ch) {
        case 'p':
            path = optarg;
            break;
        case 'o':
            if (parse_u64(optarg, &offset) != 0) {
                usage(argv[0]);
                return 1;
            }
            break;
        case 's':
            if (parse_u64(optarg, &size) != 0) {
                usage(argv[0]);
                return 1;
            }
            break;
        case 'c':
            allow_create = 1;
            break;
        case 'b':
            require_block = 1;
            break;
        case 'f':
            force = 1;
            break;
        default:
            usage(argv[0]);
            return 1;
        }
    }

    if (path == NULL) {
        report_skip("target path", "no path provided");
        printf("Summary: pass=%d fail=%d skip=%d\n",
            pass_count, fail_count, skip_count);
        return 0;
    }

    run_io_test(path, (off_t)offset, (size_t)size,
        allow_create, require_block, force);

    printf("Summary: pass=%d fail=%d skip=%d\n",
        pass_count, fail_count, skip_count);

    return (fail_count > 0) ? 1 : 0;
}
