/*
 * RISC-V 64-bit kernel entry point
 * Called from head.S after basic initialization
 */

#include <stddef.h>
#include <string.h>

#include "kernel/kernel.h"
#include "arch_proto.h"

/* External functions from other kernel modules */
extern void kmain(kinfo_t *cbi);

extern char __k_unpaged__kern_unpaged_start;
extern char __k_unpaged__kern_unpaged_end;
extern char _kernel_phys_start;
extern char _kernel_phys_end;
extern char _kernel_size;

struct riscv_bootinfo {
	u32_t magic;
	u32_t mods;
	multiboot_module_t modlist[MULTIBOOT_MAX_MODS];
};

static void riscv64_init_kinfo(void)
{
	phys_bytes mem_start, mem_size, mem_end;
	phys_bytes kernel_end;
	phys_bytes free_start, free_end;
	phys_bytes reserved_end;
	const struct riscv_bootinfo *bi;
	unsigned int i;

	memset(&kinfo, 0, sizeof(kinfo));

	kinfo.kmessages = &kmessages;
	kinfo.user_sp = (vir_bytes)USR_STACKTOP;
	kinfo.user_end = (vir_bytes)USR_DATATOP;
	kinfo.vir_kern_start = (vir_bytes)KERNEL_BASE;
	kinfo.serial_debug_baud = 115200;

	kinfo.bootstrap_start = (vir_bytes)&__k_unpaged__kern_unpaged_start;
	kinfo.bootstrap_len = (vir_bytes)&__k_unpaged__kern_unpaged_end -
		(vir_bytes)&__k_unpaged__kern_unpaged_start;
	kinfo.bootstrap_start = rounddown(kinfo.bootstrap_start, RISCV_PAGE_SIZE);
	kinfo.bootstrap_len = rounddown(kinfo.bootstrap_len, RISCV_PAGE_SIZE);

	kinfo.kernel_allocated_bytes = (phys_bytes)&_kernel_size;
	if (kinfo.kernel_allocated_bytes >= kinfo.bootstrap_len)
		kinfo.kernel_allocated_bytes -= kinfo.bootstrap_len;

	kernel_end = (phys_bytes)&_kernel_phys_end;
	reserved_end = kernel_end;

	bi = (const struct riscv_bootinfo *)RISCV_BOOTINFO_ADDR;
	if (bi->magic == RISCV_BOOTINFO_MAGIC &&
		bi->mods <= MULTIBOOT_MAX_MODS) {
		kinfo.mbi.mi_mods_count = bi->mods;
		memcpy(kinfo.module_list, bi->modlist,
			bi->mods * sizeof(bi->modlist[0]));
		kinfo.mbi.mods_addr = (u32_t)(RISCV_BOOTINFO_ADDR +
			offsetof(struct riscv_bootinfo, modlist));

		for (i = 0; i < bi->mods; i++) {
			phys_bytes mod_end = bi->modlist[i].mod_end;

			if (mod_end > reserved_end)
				reserved_end = mod_end;
		}
	}

	if (kinfo.mbi.mi_mods_count + 1 <= MULTIBOOT_MAX_MODS) {
		kinfo.kern_mod = kinfo.mbi.mi_mods_count;
		kinfo.mods_with_kernel = kinfo.mbi.mi_mods_count + 1;
		kinfo.module_list[kinfo.kern_mod].mod_start =
			(u32_t)(phys_bytes)&_kernel_phys_start;
		kinfo.module_list[kinfo.kern_mod].mod_end =
			(u32_t)(phys_bytes)&_kernel_phys_end;
		kinfo.module_list[kinfo.kern_mod].cmdline = 0;
		kinfo.module_list[kinfo.kern_mod].pad = 0;
		if (kinfo.module_list[kinfo.kern_mod].mod_end > reserved_end)
			reserved_end = kinfo.module_list[kinfo.kern_mod].mod_end;
	}

	bsp_get_memory(&mem_start, &mem_size);
	mem_end = mem_start + mem_size;
	if (reserved_end < mem_end) {
		free_start = roundup(reserved_end, RISCV_PAGE_SIZE);
		free_end = rounddown(mem_end, RISCV_PAGE_SIZE);
		if (free_end > free_start)
			add_memmap(&kinfo, free_start, free_end - free_start);
	}
}

/*===========================================================================*
 *				kernel_main			     *
 *===========================================================================*/
void kernel_main(void)
{
	/* Initialize direct console early */
	direct_init();
	direct_print("rv64: kernel_main\n");

	/* Call generic kernel main */
	riscv64_init_kinfo();
	kmain(&kinfo);
}
