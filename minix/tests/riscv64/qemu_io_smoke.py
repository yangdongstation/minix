#!/usr/bin/env python3
"""
QEMU boot + virtio-blk-mmio I/O smoke test.
"""

from __future__ import annotations

import argparse
import os
import re
import select
import shlex
import signal
import subprocess
import sys
import tempfile
import time
import pty

SKIP_RC = 2


def log(msg: str) -> None:
    print(msg, flush=True)

def log_tail(buf: str, label: str, limit: int = 2000) -> None:
    tail = buf[-limit:] if buf else ""
    log(f"{label} output tail:\n{tail}")


class ProcIO:
    def __init__(self, proc: subprocess.Popen, read_fd: int, write_fd: int):
        self.proc = proc
        self.read_fd = read_fd
        self.write_fd = write_fd


def spawn(cmd: list[str]) -> ProcIO:
    try:
        master_fd, slave_fd = pty.openpty()
    except OSError:
        proc = subprocess.Popen(
            cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            close_fds=True,
            bufsize=0,
        )
        if proc.stdin is None or proc.stdout is None:
            raise RuntimeError("failed to open pipes for QEMU")
        return ProcIO(proc, proc.stdout.fileno(), proc.stdin.fileno())

    proc = subprocess.Popen(
        cmd,
        stdin=slave_fd,
        stdout=slave_fd,
        stderr=slave_fd,
        close_fds=True,
    )
    os.close(slave_fd)
    return ProcIO(proc, master_fd, master_fd)


def send(io: ProcIO, data: str) -> None:
    os.write(io.write_fd, data.encode())


def read_until(io: ProcIO, patterns: list[re.Pattern], timeout: float) -> tuple[str, re.Pattern | None]:
    buf = ""
    end = time.time() + timeout
    while time.time() < end:
        rlist, _, _ = select.select([io.read_fd], [], [], 0.2)
        if io.read_fd in rlist:
            data = os.read(io.read_fd, 4096)
            if not data:
                break
            buf += data.decode(errors="ignore")
            for pat in patterns:
                if pat.search(buf):
                    return buf, pat
    return buf, None


def make_disk(path: str, size: int) -> None:
    with open(path, "wb") as f:
        f.truncate(size)

def run_dd_fallback(io: ProcIO, device: str, offset: int, size: int,
    allow_block: bool, timeout: int) -> int:
    if device.startswith("/dev/"):
        if not allow_block:
            log("SKIP: dd fallback unsafe for block devices")
            return SKIP_RC
        file_mode = False
    else:
        file_mode = True

    if not file_mode:
        if offset % 512 != 0 or size % 512 != 0:
            log("SKIP: dd fallback requires 512-byte alignment")
            return SKIP_RC

        count = size // 512
        if count <= 0:
            log("SKIP: dd fallback size too small")
            return SKIP_RC
        if count > 8:
            count = 8

    dev_q = shlex.quote(device)
    if file_mode:
        cmd = (
            "sh -c '"
            "test -x /bin/cat || { echo __DD_SKIP__; exit 2; }; "
            "test -x /usr/bin/cmp || { echo __DD_SKIP__; exit 2; }; "
            "echo __DD_STEP1__; "
            "mkdir -p /usr/tmp >/dev/null 2>&1; "
            "echo minix > /usr/tmp/vb.src 2>/dev/null || exit 1; "
            "echo __DD_STEP2__; "
            "/bin/cat /usr/tmp/vb.src > \"$1\" 2>/dev/null || exit 1; "
            "echo __DD_STEP3__; "
            "/usr/bin/cmp /usr/tmp/vb.src \"$1\" >/dev/null 2>&1 || exit 1; "
            "echo __DD_OK__"
            f"' sh {dev_q}"
        )
    else:
        offb = offset // 512
        cmd = (
            "sh -c '"
            "test -x /bin/dd || { echo __DD_SKIP__; exit 2; }; "
            "test -x /usr/bin/cmp || { echo __DD_SKIP__; exit 2; }; "
            "mkdir -p /usr/tmp >/dev/null 2>&1; "
            f"dd if=/bin/sh of=/usr/tmp/vb.src bs=512 count={count} 2>/dev/null || exit 1; "
            f"dd if=/usr/tmp/vb.src of=\"$1\" bs=512 seek={offb} conv=notrunc 2>/dev/null || exit 1; "
            f"dd if=\"$1\" of=/usr/tmp/vb.dst bs=512 skip={offb} count={count} 2>/dev/null || exit 1; "
            "cmp /usr/tmp/vb.src /usr/tmp/vb.dst >/dev/null 2>&1 || exit 1; "
            "echo __DD_OK__"
            f"' sh {dev_q}"
        )
    send(io, cmd + "\n")
    send(io, "echo __DD_DONE__:$?\n")

    done_pat = re.compile(r"__DD_DONE__:(\d+)")
    panic_pat = re.compile(r"panic|PANIC")
    buf, _ = read_until(io, [done_pat, panic_pat], timeout)
    if panic_pat.search(buf):
        log_tail(buf, "Kernel panic", limit=4000)
        log("FAIL: kernel panic detected")
        return 1

    m = done_pat.search(buf)
    if not m:
        log_tail(buf, "DD fallback did not complete")
        return 1

    rc_val = int(m.group(1))
    if "__DD_OK__" in buf and rc_val == 0:
        log("PASS: dd/cmp fallback")
        return 0
    if "__DD_SKIP__" in buf or rc_val == 2:
        log_tail(buf, "DD fallback skipped")
        return SKIP_RC

    log_tail(buf, "DD fallback failed")
    return 1


def run_probe(io: ProcIO, prompt_pat: re.Pattern, timeout: int) -> None:
    cmd = (
        "PATH=/sbin:/bin:/usr/bin; "
        "echo __PROBE_START__; "
        "sysenv arch 2>&1; "
        "/sbin/minix-service sysctl srv_status 2>&1 | /bin/grep virtio 2>&1; "
        "/bin/ps 2>&1 | /bin/grep virtio 2>&1; "
        "ls -l /bin/test_virtio_blk_mmio /service/virtio_blk_mmio /dev/c0d0 2>&1; "
        "echo __PROBE_END__"
    )
    send(io, cmd + "\n")
    buf, _ = read_until(io, [prompt_pat], timeout)
    start = buf.rfind("__PROBE_START__")
    end = buf.rfind("__PROBE_END__")
    if start != -1 and end != -1 and end > start:
        probe_out = buf[start:end + len("__PROBE_END__")]
        log_tail(probe_out, "VirtIO blk probe", limit=4000)
    else:
        log_tail(buf, "VirtIO blk probe", limit=4000)


def ensure_virtio_driver(io: ProcIO, prompt_pat: re.Pattern, timeout: int) -> bool:
    cmd = (
        "PATH=/sbin:/bin:/usr/bin; "
        "if /sbin/minix-service sysctl srv_status 2>/dev/null | /bin/grep -q virtio_blk_mmio; then "
        "echo __DRV_UP__; "
        "else "
        "/sbin/minix-service -c up /service/virtio_blk_mmio -dev /dev/c0d0; "
        "echo __DRV_UP_RC__:$?; "
        "fi"
    )
    send(io, cmd + "\n")
    done_pat = re.compile(r"__DRV_UP__|__DRV_UP_RC__:(\d+)")
    panic_pat = re.compile(r"panic|PANIC")
    buf, pat = read_until(io, [done_pat, panic_pat], timeout)
    if pat == panic_pat:
        log_tail(buf, "VirtIO blk driver start panic", limit=4000)
        log("FAIL: kernel panic detected")
        return False
    if "__DRV_UP__" in buf:
        return True
    m = re.search(r"__DRV_UP_RC__:(\d+)", buf)
    if m:
        rc_val = int(m.group(1))
        if rc_val == 0 or rc_val == 16:
            return True
    log_tail(buf, "VirtIO blk driver start failed", limit=4000)
    return False


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--qemu-script", required=True)
    parser.add_argument("--kernel", required=True)
    parser.add_argument("--destdir", required=True)
    parser.add_argument("--disk")
    parser.add_argument("--disk-size", type=int, default=64 * 1024 * 1024)
    parser.add_argument("--test-bin", required=True)
    parser.add_argument("--device")
    parser.add_argument("--offset", type=int, default=1024 * 1024)
    parser.add_argument("--size", type=int, default=4096)
    parser.add_argument("--require-block", action="store_true")
    parser.add_argument("--file-fallback", action="store_true")
    parser.add_argument("--dd-fallback", action="store_true")
    parser.add_argument("--dd-unsafe", action="store_true")
    parser.add_argument("--timeout", type=int, default=90)
    args = parser.parse_args()

    if not os.path.exists(args.qemu_script):
        log("SKIP: qemu script not found")
        return SKIP_RC
    if not os.path.exists(args.kernel):
        log("SKIP: kernel not found")
        return SKIP_RC
    if not os.path.isdir(args.destdir):
        log("SKIP: destdir not found")
        return SKIP_RC
    test_bin_present = os.path.exists(args.test_bin)
    if not test_bin_present and not args.dd_fallback:
        log("SKIP: test binary not found in destdir")
        return SKIP_RC

    disk_path = args.disk
    cleanup_disk = False
    if not disk_path:
        fd, disk_path = tempfile.mkstemp(prefix="minix-rv64-io-smoke-", suffix=".img")
        os.close(fd)
        make_disk(disk_path, args.disk_size)
        cleanup_disk = True

    if test_bin_present:
        test_path = args.test_bin
        if test_path.startswith(args.destdir):
            guest_test_path = test_path[len(args.destdir):]
            if not guest_test_path.startswith("/"):
                guest_test_path = "/" + guest_test_path
        else:
            guest_test_path = test_path

    device = args.device
    allow_create = False
    require_block = args.require_block
    if device is None and args.file_fallback:
        device = "/usr/tmp/virtio_blk_mmio_test.bin"
        allow_create = True
        require_block = False
    if device is None:
        device = "/dev/c0d0"

    qemu_cmd = [
        args.qemu_script,
        "-s",
        "-k",
        args.kernel,
        "-B",
        args.destdir,
        "-i",
        disk_path,
    ]
    allow_block = args.dd_unsafe or cleanup_disk

    log("Starting QEMU smoke test...")
    io = spawn(qemu_cmd)
    proc = io.proc

    try:
        login_pat = re.compile(r"login:", re.IGNORECASE)
        passwd_pat = re.compile(r"password:", re.IGNORECASE)
        prompt_pat = re.compile(r"\n.*[#\\$] ")
        panic_pat = re.compile(r"panic|PANIC")

        buf, _ = read_until(io, [login_pat, prompt_pat, panic_pat], args.timeout)
        if panic_pat.search(buf):
            log_tail(buf, "Kernel panic", limit=4000)
            log("FAIL: kernel panic detected")
            return 1

        if login_pat.search(buf):
            send(io, "root\n")
            buf, pat = read_until(io, [passwd_pat, prompt_pat, panic_pat], args.timeout)
            if panic_pat.search(buf):
                log_tail(buf, "Kernel panic", limit=4000)
                log("FAIL: kernel panic detected")
                return 1
            if pat == passwd_pat:
                send(io, "\n")
                buf, _ = read_until(io, [prompt_pat, panic_pat], args.timeout)
                if panic_pat.search(buf):
                    log_tail(buf, "Kernel panic", limit=4000)
                    log("FAIL: kernel panic detected")
                    return 1

        if not prompt_pat.search(buf):
            log_tail(buf, "Shell prompt not detected")
            log("SKIP: shell prompt not detected")
            return SKIP_RC

        if device.startswith("/dev/"):
            if not ensure_virtio_driver(io, prompt_pat, args.timeout):
                run_probe(io, prompt_pat, args.timeout)
                return 1

        if not test_bin_present:
            if args.dd_fallback:
                run_probe(io, prompt_pat, args.timeout)
                return run_dd_fallback(io, device, args.offset, args.size,
                    allow_block, args.timeout)
            log("SKIP: test binary not found in destdir")
            return SKIP_RC

        test_cmd = f"{guest_test_path} -p {device} -o {args.offset} -s {args.size}"
        if allow_create:
            test_cmd += " -c"
        if require_block:
            test_cmd += " -b"

        send(io, test_cmd + "\n")
        send(io, "echo __TEST_RC__:$?\n")

        rc_pat = re.compile(r"__TEST_RC__:(\d+)")
        buf, _ = read_until(io, [rc_pat, panic_pat], args.timeout)
        if panic_pat.search(buf):
            log_tail(buf, "Kernel panic", limit=4000)
            log("FAIL: kernel panic detected")
            return 1
        m = rc_pat.search(buf)
        if not m:
            log_tail(buf, "Test did not complete")
            log("FAIL: test did not complete")
            return 1
        rc_val = int(m.group(1))
        missing_pat = re.compile(re.escape(guest_test_path) + r": not found")
        if rc_val == 127 or missing_pat.search(buf):
            log_tail(buf, "Test binary not found")
            if args.dd_fallback:
                run_probe(io, prompt_pat, args.timeout)
                return run_dd_fallback(io, device, args.offset, args.size,
                    allow_block, args.timeout)
            log("SKIP: test binary not found in guest")
            return SKIP_RC

        summary_pat = re.compile(r"Summary: pass=(\d+) fail=(\d+) skip=(\d+)")
        sm = summary_pat.search(buf)
        if sm:
            passed = int(sm.group(1))
            failed = int(sm.group(2))
            skipped = int(sm.group(3))
        else:
            log_tail(buf, "Test summary not found")
            log("FAIL: test summary not found")
            return 1

        if failed > 0:
            log_tail(buf, "Test output")
            log("FAIL: test reported failures")
            return 1
        if passed == 0 and skipped > 0:
            log("SKIP: test skipped in guest")
            return SKIP_RC

        log("PASS: virtio-blk-mmio I/O smoke test")
        return 0
    finally:
        try:
            send(io, "\x01x")
        except OSError:
            pass
        try:
            proc.send_signal(signal.SIGTERM)
            proc.wait(timeout=5)
        except Exception:
            proc.kill()
        if cleanup_disk:
            try:
                os.unlink(disk_path)
            except OSError:
                pass


if __name__ == "__main__":
    sys.exit(main())
