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

#include <inttypes.h>

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

struct vc4_param;
struct vc4_asm;

enum op_type
  {
    ot_unknown,
    ot_cpuid,
    ot_reg,
    ot_reg_range,
    ot_reg_addr,
    ot_reg_addr_pi,
    ot_reg_addr_pd,
    ot_reg_addr_offset,
    ot_2reg_addr_begin,
    ot_2reg_addr_end,
    ot_num,
    ot_reg_shl,
    ot_reg_lsr
  };

struct op_info
{
  enum op_type type;
  expressionS exp;
  int reg, num2;
};

struct vc4_frag_option
{
  const struct vc4_asm *as;
  const struct vc4_param *param;
  uint16_t ins[5];
  bfd_reloc_code_real_type bfd_fixup;
};

struct vc4_frag_type
{
  size_t num;
  size_t cur;
  struct op_info op_inf;
  struct vc4_frag_option d[3];
};

typedef struct fix fixS;

void vc4_init_frag(fragS *f);
void vc4_init_fix(fixS *f);

#define TC_FRAG_TYPE		struct vc4_frag_type
#define TC_FRAG_INIT(fragp)	vc4_init_frag(fragp)
#define TC_FIX_TYPE             int
#define TC_INIT_FIX_DATA(fixp)  vc4_init_fix(fixp)

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


#define md_relax_frag(segment, fragp, stretch)	\
  vc4_relax_frag (segment, fragp, stretch)
extern int vc4_relax_frag(asection *, struct frag *, long);

#define LISTING_WORD_SIZE 2

