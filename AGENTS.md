# Repository Guidelines

## Project Structure & Module Organization
This repository is the full MINIX 3 source tree with an active RISC-V 64
port targeting QEMU virt.

- `minix/` holds MINIX-specific code: `kernel/`, `servers/`, `drivers/`,
  `lib/`, `commands/`, and `tests/`.
- `sys/`, `include/`, `lib/`, `bin/`, `sbin/`, and `usr.*` contain the
  NetBSD-derived base system.
- `tools/` builds the host toolchain; build outputs land under `obj/`.
- `docs/` and `README-RISCV64.md` capture porting notes and build/run details.
- `scripts/` includes build and QEMU helpers; `external/` holds third-party
  sources by license.

## Build, Test, and Development Commands
- `./build.sh`: build the full tree; add `tools` or `distribution` as targets.
- `./build.sh -m evbriscv64 distribution`: RISC-V distribution build; use the
  full flag set in `README-RISCV64.md` as the current baseline.
- `make -C minix build`: build MINIX-specific components only.
- `./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64`:
  boot in QEMU with a built kernel and destdir.
- `./minix/tests/run` or `./minix/tests/riscv64/run_tests.sh all`: run MINIX
  and RISC-V test suites; `make regression-tests` runs NetBSD-style regressions.

## Coding Style & Naming Conventions
- Follow NetBSD KNF style in `share/misc/style` (include ordering, brace
  placement, and consistent tabs).
- Prefer 80-column wrapping where practical.
- Use uppercase macros, capitalized enum values, and `uintN_t` types.
- Keep names aligned with nearby code; RISC-V tests commonly use `test_*.c`.

## Testing Guidelines
- Place tests under `minix/tests/` or `minix/tests/riscv64/` near the subsystem.
- Update the local `Makefile` entries when adding new tests.
- Include the exact test commands and results in your PR description.

## Documentation & Status Updates
- New issues go in `issue.md` with symptoms, logs, and file/line evidence.
- Update `RISC64-STATUS.md` for behavior changes or new run results.
- Append build/test changes to `docs/RISCV64_KERNEL_BUILD_LOG.md`, and adjust
  `README-RISCV64.md` if the baseline build command changes.

## Commit & Pull Request Guidelines
- Recent history uses a scoped prefix: `riscv64: ...`, `docs: ...`, `build: ...`,
  `chore: ...`. Follow `<area>: <concise summary>` in the imperative voice.
- PRs should describe the change, list build/test commands run, and attach
  relevant logs (build output or QEMU logs) when behavior changes.
