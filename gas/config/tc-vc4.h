/* tc-vc4.h -- Header file for tc-vc4.c.
   Copyright 2001, 2002, 2003, 2005, 2007 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#define TC_VC4

#define LISTING_HEADER "Vc4 GAS "

/* The target BFD architecture.  */
#define TARGET_ARCH bfd_arch_vc4

extern unsigned long vc4_machine;
#define TARGET_MACH (vc4_machine)

#define TARGET_FORMAT		"elf32-vc4"
#define TARGET_BYTES_BIG_ENDIAN	1

extern const char vc4_comment_chars [];
#define tc_comment_chars vc4_comment_chars

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB	1

#define DIFF_EXPR_OK	1	/* .-foo gets turned into PC relative relocs */

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

/* Values passed to md_apply_fix don't include the symbol value.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

#define md_apply_fix md_apply_fix

extern bfd_boolean vc4_fix_adjustable (struct fix *);
#define tc_fix_adjustable(FIX) vc4_fix_adjustable (FIX)

#define tc_gen_reloc vc4_tc_gen_reloc

/* Call md_pcrel_from_section(), not md_pcrel_from().  */
extern long md_pcrel_from_section (struct fix *, segT);
#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from_section (FIX, SEC)

/* GAS will call this function for any expression that can not be
   recognized.  When the function is called, `input_line_pointer'
   will point to the start of the expression.  */
#define md_operand(x)

#define md_relax_frag(segment, fragp, stretch) \
  vc4_relax_frag (segment, fragp, stretch)
extern int vc4_relax_frag(asection *, struct frag *, long);

#define LISTING_WORD_SIZE 2

