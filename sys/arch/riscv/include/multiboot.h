/*	$NetBSD$	*/

#ifndef _RISCV_MULTIBOOT_H_
#define	_RISCV_MULTIBOOT_H_

/*
 * Minimal multiboot definitions used by MINIX for boot parameter handling.
 */

#define MULTIBOOT_PARAM_BUF_SIZE 1024
#define MULTIBOOT_MAX_MODS	20

#ifndef __ASSEMBLY__

#include <sys/types.h>

struct multiboot_info {
	u32_t flags;
	u32_t mem_lower_unused;
	u32_t mem_upper_unused;
	u32_t boot_device;
	u32_t cmdline;
	u32_t mi_mods_count;
	u32_t mods_addr;
	u32_t mmap_length;
	u32_t mmap_addr;
	u32_t drives_length;
	u32_t drives_addr;
	u32_t config_table;
	u32_t boot_loader_name;
	u32_t apm_table;
	u32_t vbe_control_info;
	u32_t vbe_mode_info;
	u16_t vbe_mode;
	u16_t vbe_interface_seg;
	u16_t vbe_interface_off;
	u16_t vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;

struct multiboot_mod_list {
	u32_t mod_start;
	u32_t mod_end;
	u32_t cmdline;
	u32_t pad;
};
typedef struct multiboot_mod_list multiboot_module_t;

struct multiboot_mmap_entry {
	u32_t size;
	u64_t mm_base_addr;
	u64_t mm_length;
	u32_t type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

#endif /* __ASSEMBLY__ */

#endif /* _RISCV_MULTIBOOT_H_ */
