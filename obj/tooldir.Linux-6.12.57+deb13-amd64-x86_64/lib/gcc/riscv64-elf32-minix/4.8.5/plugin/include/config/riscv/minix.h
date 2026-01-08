/* Definitions for RISC-V running MINIX using ELF.
   Copyright (C) 2026 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the
Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#undef TARGET_USE_GP
#define TARGET_USE_GP 0

#undef TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_FDIV)

#undef TARGET_DEFAULT_CMODEL
#define TARGET_DEFAULT_CMODEL CM_MEDANY

#undef MINIX_TARGET_CPU_CPP_BUILTINS
#define MINIX_TARGET_CPU_CPP_BUILTINS()		\
  do							\
    {							\
      if (!TARGET_HARD_FLOAT_ABI)			\
        builtin_define ("_SOFT_FLOAT");		\
    }							\
  while (0)
