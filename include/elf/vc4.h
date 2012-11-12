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
  RELOC_NUMBER (R_VC4_PCREL_27_MUL2,  7)
  RELOC_NUMBER (R_VC4_PCREL_32,       8)
  RELOC_NUMBER (R_VC4_IMM_5,          9)
  RELOC_NUMBER (R_VC4_IMM_6,          10)
  RELOC_NUMBER (R_VC4_IMM_11,         11)
  RELOC_NUMBER (R_VC4_IMM_16,         12)
  RELOC_NUMBER (R_VC4_IMM_23,         13)
  RELOC_NUMBER (R_VC4_IMM_27,         14)
  RELOC_NUMBER (R_VC4_IMM_32,         15)
  RELOC_NUMBER (R_VC4_IMM_32_2,       16)

  RELOC_NUMBER (R_VC4_GNU_VTINHERIT,  30)
  RELOC_NUMBER (R_VC4_GNU_VTENTRY,    31)

  RELOC_NUMBER (R_VC4_max,       32)
END_RELOC_NUMBERS(R_VC4_MAX)
        
#endif /* _ELF_VC4_H */
