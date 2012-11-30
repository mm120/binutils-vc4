/* VC4 ELF support for BFD.
   Copyright 2012 Free Software Foundation, Inc.
   Contributed by Mark Marshall, markmarshall14@gmail.com

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

#ifndef VC4_OPCODE_H__
#define VC4_OPCODE_H__

#include <stdlib.h>
#include <stdio.h>

#include "../../libvc4/vc4.h"
#include "../../libvc4/eval.h"

struct vc4_info *vc4_info;

void vc4_load_opcode_info(void);

#endif /* VC4_OPCODE_H__ */
