/* VC$ ELF support for BFD.
   Copyright 2007, 2010 Free Software Foundation, Inc.
   Contributed by M R Swami Reddy.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _ELF_VC4_H
#define _ELF_VC4_H

#include "elf/reloc-macros.h"

/* Creating indices for reloc_map_index array.  */
START_RELOC_NUMBERS(elf_vc4_reloc_type)
  RELOC_NUMBER (R_VC4_NONE,           0)
  RELOC_NUMBER (R_VC4_PCREL_7_MUL2,   1)
  RELOC_NUMBER (R_VC4_PCREL_8_MUL2,   2)
  RELOC_NUMBER (R_VC4_PCREL_10_MUL2,  3)
  RELOC_NUMBER (R_VC4_PCREL_16,       4)
  RELOC_NUMBER (R_VC4_PCREL_23_MUL2,  5)
  RELOC_NUMBER (R_VC4_PCREL_27,       6)
  RELOC_NUMBER (R_VC4_PCREL_32,       7)

  RELOC_NUMBER (R_VC4_ABS_32,         8)
  RELOC_NUMBER (R_VC4_GNU_VTINHERIT,  30)
  RELOC_NUMBER (R_VC4_GNU_VTENTRY,    31)
  /*
  RELOC_NUMBER (R_VC4_NUM8,           1)
  RELOC_NUMBER (R_VC4_NUM16,          2)
  RELOC_NUMBER (R_VC4_NUM32,          3)
  RELOC_NUMBER (R_VC4_NUM32a,         4)
  RELOC_NUMBER (R_VC4_REGREL4,        5)
  RELOC_NUMBER (R_VC4_REGREL4a,       6)
  RELOC_NUMBER (R_VC4_REGREL14,       7)
  RELOC_NUMBER (R_VC4_REGREL14a,      8)
  RELOC_NUMBER (R_VC4_REGREL16,       9)
  RELOC_NUMBER (R_VC4_REGREL20,       10)
  RELOC_NUMBER (R_VC4_REGREL20a,      11)
  RELOC_NUMBER (R_VC4_ABS20,          12)
  RELOC_NUMBER (R_VC4_ABS24,          13)
  RELOC_NUMBER (R_VC4_IMM4,           14)
  RELOC_NUMBER (R_VC4_IMM8,           15)
  RELOC_NUMBER (R_VC4_IMM16,          16)
  RELOC_NUMBER (R_VC4_IMM20,          17)
  RELOC_NUMBER (R_VC4_IMM24,          18)
  RELOC_NUMBER (R_VC4_IMM32,          19)
  RELOC_NUMBER (R_VC4_IMM32a,         20)
  RELOC_NUMBER (R_VC4_DISP4,          21)
  RELOC_NUMBER (R_VC4_DISP8,          22)
  RELOC_NUMBER (R_VC4_DISP16,         23)
  RELOC_NUMBER (R_VC4_DISP24,         24)
  RELOC_NUMBER (R_VC4_DISP24a,        25)
  RELOC_NUMBER (R_VC4_SWITCH8,        26)
  RELOC_NUMBER (R_VC4_SWITCH16,       27)
  RELOC_NUMBER (R_VC4_SWITCH32,       28)
  RELOC_NUMBER (R_VC4_GOT_REGREL20,   29)
  RELOC_NUMBER (R_VC4_GOTC_REGREL20,  30)
  RELOC_NUMBER (R_VC4_GLOB_DAT,       31)
  */
  RELOC_NUMBER (R_VC4_max,       32)
END_RELOC_NUMBERS(R_VC4_MAX)
        
#endif /* _ELF_VC4_H */
