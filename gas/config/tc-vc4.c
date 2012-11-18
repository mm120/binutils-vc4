/* tc-vc4.c -- Assembler for the Vc4 family.
   Copyright 2001, 2002, 2003, 2005, 2006, 2007, 2009
   Free Software Foundation.
   Contributed by Johan Rydberg, jrydberg@opencores.org

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

#include "as.h"
#include "subsegs.h"
#include "symcat.h"
#include "vc4.h"
#include "safe-ctype.h"
#include <inttypes.h>
#include <limits.h>
#include <assert.h>
#include "elf/vc4.h"

const char comment_chars[]        = "#";
const char line_comment_chars[]   = "#";
const char line_separator_chars[] = ";";
const char EXP_CHARS[]            = "eE";
const char FLT_CHARS[]            = "dD";


#define VC4_SHORTOPTS "m:"
const char * md_shortopts = VC4_SHORTOPTS;

struct option md_longopts[] =
{
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof (md_longopts);

unsigned long vc4_machine = 0; /* default */

int
md_parse_option (int c ATTRIBUTE_UNUSED, char * arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE * stream ATTRIBUTE_UNUSED)
{
}

/*
static void
ignore_pseudo (int val ATTRIBUTE_UNUSED)
{
  discard_rest_of_line ();
}
*/

const char vc4_comment_chars[] = ";#";

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

#define UNUSED(x) ((void)&x)


struct vc4_info *vc4_info;


static struct hash_control *vc4_hash;

char *dump_asm_name(const struct vc4_asm *a, char *buf);

char *dump_asm_name(const struct vc4_asm *a, char *buf)
{
  switch (a->pat.count) {
  case 0:
  default:
    sprintf(buf, "%s", a->str);
    break;
  case 1:
    sprintf(buf, "%s[%c:%u]", a->str,
	    a->pat.pat[0].code, a->pat.pat[0].val);
    break;
  case 2:
    sprintf(buf, "%s[%c:%u %c:%u]", a->str,
	    a->pat.pat[0].code, a->pat.pat[0].val,
	    a->pat.pat[1].code, a->pat.pat[1].val);
    break;
  case 3:
    sprintf(buf, "%s[%c:%u %c:%u %c:%u]", a->str,
	    a->pat.pat[0].code, a->pat.pat[0].val,
	    a->pat.pat[1].code, a->pat.pat[1].val,
	    a->pat.pat[2].code, a->pat.pat[2].val);
    break;
  }
  return buf;
}


void
md_begin (void)
{
  struct vc4_asm *a;

  if (vc4_info == NULL) {
    vc4_info = vc4_read_arch_file(
      "/home/marmar01/src/rpi/videocoreiv/videocoreiv.arch");
  }
  vc4_get_opcodes(vc4_info);

  vc4_hash = hash_new();

  for (a = vc4_info->all_asms; a != NULL; a = a->next_all) {

    struct vc4_asm *o = (struct vc4_asm *) hash_find(vc4_hash, a->str);
    a->next = o;
    hash_jam(vc4_hash, a->str, a);
  }

  bfd_set_arch_mach(stdoutput, TARGET_ARCH, bfd_mach_vc4);
}

static inline char *
skip_space (char *s)
{
  while (*s == ' ' || *s == '\t')
    ++s;
  return s;
}

/* Extract one word from FROM and copy it to TO.  */

static char *
extract_word (char *from, char *to, int limit)
{
  char *op_end;
  int size = 0;

  /* Drop leading whitespace.  */
  from = skip_space (from);
  *to = 0;

  /* Find the op code end.  */
  for (op_end = from; *op_end != 0 && is_part_of_name (*op_end);)
    {
      to[size++] = *op_end++;
      if (size + 1 >= limit)
	break;
    }

  to[size] = 0;
  return op_end;
}

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
  size_t width;
  int reg, num2;
};

static char *exp_print(const expressionS *exp, char *buf)
{
  const char *add;
  const char *op;

  if (exp == NULL) {
    strcpy(buf, "");
    return buf;
  }

  add = (exp->X_add_symbol && S_GET_NAME(exp->X_add_symbol)) ?
    S_GET_NAME(exp->X_add_symbol) : "";
  op = (exp->X_op_symbol && S_GET_NAME(exp->X_op_symbol)) ?
    S_GET_NAME(exp->X_op_symbol) : "";

  switch (exp->X_op) {
  case O_constant:
    assert(exp->X_op_symbol == NULL);
    assert(exp->X_add_symbol == NULL);
    sprintf(buf, "{%s %u:0x%08x}", "O_constant",
	    (unsigned)exp->X_add_number,
	    (unsigned)exp->X_add_number);
    break;

  case O_symbol:
    assert(exp->X_op_symbol == NULL);
    if (exp->X_add_number == 0) {
      sprintf(buf, "{%s %p:%s}", "O_symbol",
	      exp->X_add_symbol, add);
    } else {
      sprintf(buf, "{%s %p:%s %u:0x%08x}", "O_symbol",
	      exp->X_add_symbol, add,
	      (unsigned)exp->X_add_number,
	      (unsigned)exp->X_add_number);
    }
    break;

  case O_subtract:
    if (exp->X_add_number == 0) {
      sprintf(buf, "{%s %p:%s %p:%s}", "O_subtract",
	      exp->X_add_symbol, add,
	      exp->X_op_symbol, op);
    } else {
      sprintf(buf, "{%s %p:%s %p:%s %u:0x%08x}", "O_subtract",
	      exp->X_add_symbol, add,
	      exp->X_op_symbol, op,
	      (unsigned)exp->X_add_number,
	      (unsigned)exp->X_add_number);
    }
    break;

  default:
    sprintf(buf, "{%d %p:%s %p:%s %u:0x%08x}",
	    exp->X_op,
	    exp->X_add_symbol, add,
	    exp->X_op_symbol, op,
	    (unsigned)exp->X_add_number,
	    (unsigned)exp->X_add_number);
    break;
  }
  return buf;
}

static char *print_op_info(const struct op_info *inf, char *buf)
{
  char buf2[256];

  switch (inf->type)
    {
    case ot_unknown:         strcpy(buf, "<unknown>"); break;
    case ot_cpuid:           strcpy(buf, "cpuid"); break;
    case ot_reg:             sprintf(buf, "r%d", inf->reg); break;
    case ot_reg_range:       sprintf(buf, "r%d-r%d", inf->reg, inf->num2); break;
    case ot_reg_addr:        sprintf(buf, "(r%d)", inf->reg); break;
    case ot_reg_addr_pi:     sprintf(buf, "(r%d)++", inf->reg); break;
    case ot_reg_addr_pd:     sprintf(buf, "--(r%d)", inf->reg); break;
    case ot_2reg_addr_begin: sprintf(buf, "(r%d", inf->reg); break;
    case ot_2reg_addr_end:   sprintf(buf, "r%d)", inf->reg); break;

    case ot_reg_shl:
      sprintf(buf, "r%d shl %s",
	      inf->reg, exp_print(&inf->exp, buf2));
      break;

    case ot_reg_lsr:
      sprintf(buf, "r%d lsr %s",
	      inf->reg, exp_print(&inf->exp, buf2));
      break;

    case ot_reg_addr_offset:
      sprintf(buf, "%s [width:%u] (r%d)",
	      exp_print(&inf->exp, buf2), inf->width, inf->reg);
      break;

    case ot_num:
      sprintf(buf, "%s [width:%u]",
	      exp_print(&inf->exp, buf2), inf->width);
      break;

    default:
      printf("asdfasld999fk %s %d\n", __FUNCTION__, __LINE__);
      assert(0);
      break;
    }

  return buf;
}


static char *match_reg(char *str, int *num)
{
  int r;
  int l = -1;
  int reg;

  if (str[0] == 'r' && ISDIGIT(str[1])) {

    r = sscanf(str, "r%d%n", &reg, &l);

    if (r >= 1 && l > 0 && !ISALNUM(str[l])) {
      str += l;
      str = skip_space(str);
      *num = reg;
      return str;
    }
  }

  if (str[0] == 's' && str[1] == 'p' && !ISALNUM(str[2])) {
    *num = 25;
    str += 2;
    str = skip_space(str);
    return str;
  }
  if (str[0] == 'l' && str[1] == 'r' && !ISALNUM(str[2])) {
    *num = 26;
    str += 2;
    str = skip_space(str);
    return str;
  }
  if (str[0] == 's' && str[1] == 'r' && !ISALNUM(str[2])) {
    *num = 30;
    str += 2;
    str = skip_space(str);
    return str;
  }
  if (str[0] == 'p' && str[1] == 'c' && !ISALNUM(str[2])) {
    *num = 31;
    str += 2;
    str = skip_space(str);
    return str;
  }
  return NULL;
}

static uint32_t vc4_log2(uint32_t v)
{
  uint32_t w = 32;
  if (v == 0)
    return 1;
  while (w > 2) {
    if ((v ^ (v << 1)) & 0x80000000) {
      return w;
    }
    v <<= 1;
    w--;
  }
  return w;
}

static char *vc4_get_operand(char *str, struct op_info *inf)
{
  int reg, reg2;
  int pre_dec = 0;
  int post_inc = 0;

  char *cont;

  cont = match_reg(str, &reg);

  if (cont != NULL) {
    str = cont;
    if (*str == ',' || *str == 0) {
      inf->type = ot_reg;
      inf->reg = reg;
      return str;
    }
    else if (*str == '-') {
      str++;
      str = skip_space(str);

      cont = match_reg(str, &reg2);

      if (cont != NULL) {
	str = cont;

	inf->type = ot_reg_range;
	inf->reg = reg;
	inf->num2 = reg2;
	return str;
      }
    }
    else if (strncmp(str, "shl", 3) == 0 && !ISALNUM(str[3])) {
      str += 3;
      str = skip_space(str);

      input_line_pointer = str;
      expression/*_and_evaluate*/(&inf->exp);
      if (inf->exp.X_op != O_absent) {
	str = input_line_pointer;

	inf->type = ot_reg_shl;
	if (inf->exp.X_op == O_constant)
	  inf->width = vc4_log2(inf->exp.X_add_number);
	else
	  inf->width = 32;
	inf->reg = reg;
	return str;
      }
    }
    else if (strncmp(str, "lsr", 3) == 0 && !ISALNUM(str[3])) {
      str += 3;
      str = skip_space(str);
      
      input_line_pointer = str;
      expression/*_and_evaluate*/(&inf->exp);
      if (inf->exp.X_op != O_absent) {
	str = input_line_pointer;

	inf->type = ot_reg_lsr;
	if (inf->exp.X_op == O_constant)
	  inf->width = vc4_log2(inf->exp.X_add_number);
	else
	  inf->width = 32;
	inf->reg = reg;
	return str;
      }
    }
    else if (*str == ')') {
      str++;
      inf->type = ot_2reg_addr_end;
      inf->reg = reg;
      return str;
    } else {
      return NULL;
    }
  }

  if (str[0] == '-' && str[1] == '-') {
    pre_dec = 1;
    str += 2;
  }

  if (str[0] == '(') {
    str += 1;
    cont = match_reg(str, &reg);

    if (cont != NULL) {
      str = cont;
      if (*str == ')') {
	str++;
	if (str[0] == '+' && str[1] == '+') {
	  post_inc = 1;
	  str += 2;
	}
	if (*str == ',' || *str == 0) {
	  inf->type =
	    pre_dec ? ot_reg_addr_pd :
	    post_inc ? ot_reg_addr_pi :
	    ot_reg_addr;
	  inf->reg = reg;
	  return str;
	}
      } else if (*str == ',') {
	  inf->type = ot_2reg_addr_begin;
	  inf->reg = reg;
	  return str;
      }
    }
  }

  if (strcmp(str, "cpuid") == 0 && !ISALNUM(str[5])) {
    str += 5;
    inf->type = ot_cpuid;
    return str;
  }

  input_line_pointer = str;
  expression/*_and_evaluate*/(&inf->exp);
  if (inf->exp.X_op != O_absent) {
    str = input_line_pointer;

    if (*str == '(') {
      str += 1;
      cont = match_reg(str, &reg);

      if (cont != NULL) {
	str = cont;
	if (*str == ')') {
	  str++;
	  if (*str == ',' || *str == 0) {
	    inf->type = ot_reg_addr_offset;
	    inf->reg = reg;
	    inf->width = 32;
	    return str;
	  }
	}
      }
    } else if (*str == ',' || *str == 0) {
      inf->type = ot_num;
      if (inf->exp.X_op == O_constant)
	inf->width = vc4_log2(inf->exp.X_add_number);
      else
	inf->width = 32;
      return str;
    }
  }

  return NULL;
}

static int
vc4_operands (char **line, struct op_info *ops)
{
  char *str = skip_space(*line);
  int i;
  const int max_ops = 5;
  char buf[256];

  if (*str == 0) {
    *line = str;
    return 0;
  }

  for (i=0; i<max_ops; i++) {

    if (i != 0) {
      if (*str != ',') {
	return -1;
      }
      str++;
    }

    str = vc4_get_operand(str, &ops[i]);
    if (str == NULL) {
      return -1;
    }
    printf("OP%d = %s\n", i+1, print_op_info(&ops[i], buf));
    if (*str == 0) {
      *line = str;
      return i + 1;
    }
  }

  return -1;
}

#include <ctype.h>

struct match_ops {
  enum vc4_param_type pt;
  enum op_type ot;
  uint32_t reg_bitmap;
  int width_match;
};

static const struct match_ops match_ops_data[] =
{
  { vc4_p_reg_0_31,             ot_reg,         	0, 0 },
  { vc4_p_reg_0_15,             ot_reg,         	0xFFFFu, 0 },
  { vc4_p_reg_0_6_16_24,        ot_reg,         	(1u << 0) | (1u << 6) | (1u << 16) | (1u << 24), 0 },
  { vc4_p_reg_r6,               ot_reg,         	(1u << 6), 0 },
  { vc4_p_reg_sp,               ot_reg,        	 	(1u << 25), 0 },
  { vc4_p_reg_lr,               ot_reg,        	 	(1u << 26), 0 },
  { vc4_p_reg_sr,               ot_reg,         	(1u << 30), 0 },
  { vc4_p_reg_pc,               ot_reg,         	(1u << 31), 0 },
  { vc4_p_reg_cpuid,            ot_cpuid,       	0, 0 },
  { vc4_p_reg_range,            ot_reg_range,   	(1u << 0) | (1u << 6) | (1u << 16) | (1u << 24), 0 },
  { vc4_p_reg_range_r6,         ot_reg_range,   	(1u << 6) , 0 },
  { vc4_p_reg_shl,              ot_reg_shl,     	0, 0 },
  { vc4_p_reg_shl_p1,           ot_reg_shl,     	0, 0 },
  { vc4_p_reg_lsr,              ot_reg_lsr,     	0, 0 },
  { vc4_p_reg_lsr_p1,           ot_reg_lsr,     	0, 0 },
  { vc4_p_reg_shl_8,            ot_reg_shl,     	0, 0 },
  { vc4_p_num_u,                ot_num,         	0, 1 },
  { vc4_p_num_s,                ot_num,         	0, 1 },
  { vc4_p_num_u4,               ot_num,         	0, 4 },
  { vc4_p_num_s4,               ot_num,         	0, 4 },
  { vc4_p_addr_reg_0_15,        ot_reg_addr,    	0xFFFFu, 0 },
  { vc4_p_addr_reg_0_31,        ot_reg_addr,    	0, 0 },
  { vc4_p_addr_2reg_begin_0_31, ot_2reg_addr_begin, 	0, 0 },
  { vc4_p_addr_2reg_end_0_31,   ot_2reg_addr_end, 	0, 0 },
  { vc4_p_addr_reg_num_u,       ot_reg_addr,    	0, 0 },
  { vc4_p_addr_reg_num_u,       ot_reg_addr_offset,	0, 1 },
  { vc4_p_addr_reg_num_s,       ot_reg_addr,    	0, 0 },
  { vc4_p_addr_reg_num_s,       ot_reg_addr_offset,     0, 1 },
  { vc4_p_addr_reg_0_15_num_u4, ot_reg_addr,    	0xFFFFu, 0 },
  { vc4_p_addr_reg_0_15_num_u4, ot_reg_addr_offset,	0xFFFFu, 4 },
  { vc4_p_addr_reg_0_15_num_s4, ot_reg_addr,   	 	0xFFFFu, 0 },
  { vc4_p_addr_reg_0_15_num_s4, ot_reg_addr_offset, 	0xFFFFu, 4 },
  { vc4_p_addr_reg_post_inc,    ot_reg_addr_pi, 	0, 0 },
  { vc4_p_addr_reg_pre_dec,     ot_reg_addr_pd, 	0, 0 },
  { vc4_p_r0_rel_s,             ot_reg_addr_offset, 	(1u << 0), 1 },
  { vc4_p_r0_rel_s2,            ot_reg_addr_offset, 	(1u << 0), 2 },
  { vc4_p_r0_rel_s4,            ot_reg_addr_offset, 	(1u << 0), 4 },
  { vc4_p_r24_rel_s,            ot_reg_addr_offset, 	(1u << 24), 1 },
  { vc4_p_r24_rel_s2,           ot_reg_addr_offset, 	(1u << 24), 2 },
  { vc4_p_r24_rel_s4,           ot_reg_addr_offset, 	(1u << 24), 4 },
  { vc4_p_sp_rel_s,             ot_reg_addr_offset, 	(1u << 25), 1 },
  { vc4_p_sp_rel_s2,            ot_reg_addr_offset, 	(1u << 25), 2 },
  { vc4_p_sp_rel_s4,            ot_reg_addr_offset, 	(1u << 25), 4 },
  { vc4_p_pc_rel_s,             ot_num, 		0, 1 },
  { vc4_p_pc_rel_s2,            ot_num, 		0, 2 },
  { vc4_p_pc_rel_s4,            ot_num, 		0, 4 },
  /*
  { vc4_p_pc_rel_s,             ot_reg_addr_offset, 	(1u << 31), 1 },
  { vc4_p_pc_rel_s2,            ot_reg_addr_offset, 	(1u << 31), 2 },
  { vc4_p_pc_rel_s4,            ot_reg_addr_offset, 	(1u << 31), 4 },
  */
};

static int match_op_info_to_vc4_asm_item(struct vc4_param *param,
					 struct op_info *ops)
{
  size_t i;
  /*
  char buf[2][256];

  printf("match %s = %s\n",
	 vc4_param_print(param, buf[0]),
	 print_op_info(ops, buf[1]));
  */
  for (i = 0; i < ARRAY_SIZE(match_ops_data); i++) {

    if (match_ops_data[i].pt == param->type &&
	match_ops_data[i].ot == ops->type &&
	(match_ops_data[i].reg_bitmap == 0 ||
	 match_ops_data[i].reg_bitmap & (1u << ops->reg))) {

      if (match_ops_data[i].width_match == 0)
	return 0;
      if (match_ops_data[i].width_match == 2 &&
	  ops->exp.X_op == O_constant &&
	  (ops->exp.X_add_number & 0x1))
	return -1;
      if (match_ops_data[i].width_match == 4 &&
	  ops->exp.X_op == O_constant &&
	  (ops->exp.X_add_number & 0x3))
	return -1;
      if (ops->width <= param->num_width)
	return 0;
      return ops->width - param->num_width;
    }
  }

  return -1;
}


static struct vc4_asm *match_op_info_to_vc4_asm(size_t min_size,
						size_t count,
						struct op_info *ops,
						struct vc4_asm *list)
{
  struct vc4_asm *opcode;
  struct vc4_asm *best;
  int best_error;
  int this_error;
  int ret;
  size_t i;
  int ers[5];
  char buf[2][256];

  best = NULL;
  best_error = INT_MAX;

  for (opcode = list; opcode != NULL; opcode = opcode->next) {

    if (opcode->op->num_params != count)
      continue;

    if (opcode->op->length < min_size)
      continue;

    memset(ers, 0, sizeof(ers));

    this_error = 0;
    for (i=0; i<count; i++) {
      ret = match_op_info_to_vc4_asm_item(&opcode->op->params[i], &ops[i]);
      ers[i] = ret;
      if (ret < 0) {
	this_error = -1;
	break;
      }
      this_error += ret;
    }

    printf("opcode = %-10s %s %s\n",
	   opcode->str,
	   opcode->op->format,
	   opcode->op->string);

    {
      printf("  ");
      for (i=0; i<opcode->pat.count; i++) {
	printf(" (%c/%u)",
	       opcode->pat.pat[i].code,
	       opcode->pat.pat[i].val);
      }
      for (i=0; i<opcode->op->num_params; i++) {
	printf(" %s <> %s = %d",
	       vc4_param_print(&opcode->op->params[i], buf[0]),
	       print_op_info(&ops[i], buf[1]),
	       ers[i]);
      }
      printf(" => %d\n", this_error);
    }

    if (this_error >= 0 /*&& this_error < best_error*/ &&
	(best == NULL || best->op->length > opcode->op->length)) {
      best_error = this_error;
      best = opcode;
    }
  }

  (void)&best_error;

  return best;
}

static void fill_value(uint16_t *ins, const struct vc4_opcode *op,
		       char code, uint32_t val)
{
  uint16_t mask;
  uint16_t *p;
  const char *f;

  printf("Fill %s %c %u %d\n", op->string,
	 code, val, op->vals[code - 'a'].length);

  assert(strlen(op->string) == 16 * op->length);
  assert(code >= 'a' && code <= 'z');

  if (op->vals[code - 'a'].length == 32) {
    val = ((val >> 16) & 0xffff) | ((val & 0xffff) << 16);
  }

  mask = 0x0000;
  p = ins + op->length;
  f = op->string + 16 * op->length;

  assert(*f == 0);

  while (p >= ins) {
    if (mask == 0) {
      mask = 0x0001;
      p--;
    }

    if (*--f == code) {
      *p &= ~mask;
      if (val & 1) {
	*p |= mask;
      }
      val >>= 1;
    }
    mask <<= 1;
  }
}

struct fixup_op_info {
  int set;
  int broken;
  expressionS exp;
  size_t width;
  int divide;
  int pc_rel;
  struct vc4_asm *opcode;
  struct op_info *op;
  struct vc4_param *param;
};


static void fixup_num(struct fixup_op_info *roi, int where)
{
  char buf2[256];
  bfd_reloc_code_real_type bfd_fixup;

  if (!roi->set)
    return;

  if (roi->exp.X_op == O_symbol) {

    bfd_fixup = vc4_bfd_fixup_get(roi->opcode->op->string,
				  roi->param->num_code,
				  roi->pc_rel,
				  roi->divide);

    if (bfd_fixup == 0) {
      as_bad("%s: Can't find bfd fixup type! %d %s\n", __func__,
	     roi->width, print_op_info(roi->op, buf2));
      return;
    }

    printf("%s: fix_new_exp %s %d %d/%d %d %d %llx%llx %s\n", __func__,
	   roi->pc_rel ? "pc-rel" : "imm", bfd_fixup,
	   roi->width,
	   vc4_bfd_fixup_get_width(bfd_fixup),
	   vc4_bfd_fixup_get_length(bfd_fixup),
	   vc4_bfd_fixup_get_divide(bfd_fixup),
	   vc4_bfd_fixup_get_mask(bfd_fixup).hi,
	   vc4_bfd_fixup_get_mask(bfd_fixup).lo,
	   print_op_info(roi->op, buf2));

    fix_new_exp(frag_now, where, roi->opcode->op->length * 2,
		&roi->op->exp, roi->pc_rel, bfd_fixup);
  }
}

static int get_reg_div8(int reg)
{
  int x = 0;
  switch(reg) {
  case 0: x = 0; break;
  case 6: x = 1; break;
  case 16: x = 2; break;
  case 24: x = 3; break;
  default:
    as_fatal("A instruction expecting r0-, r6-, r16- or r24- was passed r%u-",
	     reg);
    break;
  }
  return x;
}

static void output_num(struct fixup_op_info *roi, uint16_t *ins,
		       struct vc4_asm *opcode,
		       struct op_info *op, struct vc4_param *param,
		       int pc_rel, int divide)
{
  if (op->exp.X_op == O_constant) {
    if (op->exp.X_add_number > (1LL << param->num_width)) {
      roi->broken = 1;
    }
    if ((divide > 1) && (op->exp.X_add_number % divide)) {
      roi->broken = 1;
    }
    fill_value(ins, opcode->op, param->num_code,
	       op->exp.X_add_number / divide);
  } else {
    if (roi->set)
      as_bad("Can't have two relax op's");
    
    roi->set = 1;
    roi->exp = op->exp;
    roi->width = param->num_width;
    roi->divide = divide;
    roi->pc_rel = pc_rel;
    roi->opcode = opcode;
    roi->op = op;
    roi->param = param;
  }
}

void
md_assemble (char * str)
{
  char op[11];
  struct vc4_asm *opcode;
  struct vc4_asm *list;
  size_t i;
  char *t;
  struct op_info ops[5];
  int count;
  uint16_t ins[5];
  char buf[256];
  char *frag;
  int where;
  struct fixup_op_info fixup_info;
  size_t min_size;

  printf("A %s\n", str);

  str = skip_space (extract_word (str, op, sizeof (op)));

  if (!op[0])
    as_bad (_("can't find opcode "));

  list = (struct vc4_asm *) hash_find (vc4_hash, op);

  /*
  for (opcode = list; opcode != NULL; opcode = opcode->next) {
    printf("opcode = %-10s %s\n", opcode->str, opcode->op->format);
    if (opcode->op->num_params != 0 || opcode->pat.count != 0) {
      printf("  ");
      for (i=0; i<opcode->pat.count; i++) {
	printf(" (%c/%u)",
	       opcode->pat.pat[i].code,
	       opcode->pat.pat[i].val);
      }
      for (i=0; i<opcode->op->num_params; i++) {
	printf(" %s", vc4_param_print(&opcode->op->params[i], buf));
      }
      printf("\n");
    }
  }
  */

  t = input_line_pointer;

  memset(ops, 0, sizeof(ops));

  count = vc4_operands(&str, ops);
  if (*skip_space (str) || count < 0) {
    printf("[%s/%s] %d\n", str, skip_space (str), count);
    as_bad (_("garbage at end of line"));
    return;
  }
  input_line_pointer = t;

  min_size = 0;
 try_again:

  opcode = match_op_info_to_vc4_asm(min_size, count, ops, list);
  if (opcode == NULL) {
    as_bad (_("can't match operands to opcode"));
    return;
  }

  printf(">> opcode = %-10s %s (%s) %s %s\n",
	 opcode->str, opcode->op->format,
	 opcode->op->string, dump_asm_name(opcode, buf),
	 opcode->relax_bigger ? "<relax?>" : "");

  ins[0] = opcode->ins[0];
  ins[1] = opcode->ins[1];
  ins[2] = ins[3] = ins[4] = 0;

  memset(&fixup_info, 0, sizeof(fixup_info));

  fixup_info.set = 0;

  for (i=0; i<opcode->op->num_params; i++) {

    switch (opcode->op->params[i].type) {
    case vc4_p_reg_0_6_16_24:
      fill_value(ins, opcode->op, opcode->op->params[i].reg_code,
		 get_reg_div8(ops[i].reg));
      break;

    case vc4_p_reg_range:
      fill_value(ins, opcode->op, opcode->op->params[i].reg_code,
		 get_reg_div8(ops[i].reg));
      fill_value(ins, opcode->op, opcode->op->params[i].num_code,
		 (ops[i].num2 - ops[i].reg) & 31);
      break;

    case vc4_p_reg_range_r6:
      assert(ops[i].reg == 6);
      fill_value(ins, opcode->op, opcode->op->params[i].num_code,
		 (ops[i].num2 - ops[i].reg) & 31);
      break;

    case vc4_p_reg_shl:
    case vc4_p_reg_lsr:
      if (ops[i].exp.X_op == O_constant) {
	fill_value(ins, opcode->op, opcode->op->params[i].num_code,
		   ops[i].exp.X_add_number);
      } else {
	assert(0 && "Need a constant for reg_shl / reg_lsr");
      }
      break;

    case vc4_p_reg_shl_p1:
    case vc4_p_reg_lsr_p1:
      if (ops[i].exp.X_op == O_constant) {
	fill_value(ins, opcode->op, opcode->op->params[i].num_code,
		   ops[i].exp.X_add_number - 1);
      } else {
	assert(0 && "Need a constant for reg_shl / reg_lsr");
      }
      break;

    default:
      break;
    }

    if (vc4_param_has_reg(opcode->op->params[i].type)) {
      assert(ops[i].reg < (1 << opcode->op->params[i].reg_width));
      fill_value(ins, opcode->op, opcode->op->params[i].reg_code, ops[i].reg);
    }

    if (vc4_param_has_num(opcode->op->params[i].type)) {
      output_num(&fixup_info, ins, opcode, &ops[i], &opcode->op->params[i],
		 vc4_param_pc_rel(opcode->op->params[i].type),
		 vc4_param_divide(opcode->op->params[i].type));
    }
  }

  if (fixup_info.broken) {
    if (opcode->op->length < 5) {
      min_size = opcode->op->length + 1;
      goto try_again;
    } else {
      printf("?????????????????\n");
      as_bad("Failed to fit instruction operand!\n");
    }
  }

  dwarf2_emit_insn (0);

  if (opcode->relax_bigger && fixup_info.set) {
    frag = frag_more(opcode->op->length * 2);
    where = frag - frag_now->fr_literal;
  } else {
    frag = frag_more(opcode->op->length * 2);
    where = frag - frag_now->fr_literal;
  }

  fixup_num(&fixup_info, where);

  for (i=0; i<opcode->op->length; i++) {
    printf("%04x\n", ins[i]);
    bfd_putl16 ((bfd_vma)ins[i], frag + i * 2);
  }
}

valueT
md_section_align (segT segment, valueT size)
{
  int align = bfd_get_section_alignment (stdoutput, segment);
  return ((size + (1 << align) - 1) & (-1 << align));
}

symbolS *
md_undefined_symbol (char * name ATTRIBUTE_UNUSED)
{
  return 0;
}


/* Interface to relax_segment.  */

int vc4_relax_frag(asection *s, struct frag *f, long l)
{
  (void)s;
  (void)f;
  (void)&l;
  return 0;
}

/* FIXME: Look through this.  */

const relax_typeS md_relax_table[] =
{
/* The fields are:
   1) most positive reach of this state,
   2) most negative reach of this state,
   3) how many bytes this mode will add to the size of the current frag
   4) which index into the table to try if we can't fit into this one.  */

  /* The first entry must be unused because an `rlx_more' value of zero ends
     each list.  */
  {1, 1, 0, 0},

  /* The displacement used by GAS is from the end of the 2 byte insn,
     so we subtract 2 from the following.  */
  /* 16 bit insn, 8 bit disp -> 10 bit range.
     This doesn't handle a branch in the right slot at the border:
     the "& -4" isn't taken into account.  It's not important enough to
     complicate things over it, so we subtract an extra 2 (or + 2 in -ve
     case).  */
  {511 - 2 - 2, -512 - 2 + 2, 0, 2 },
  /* 32 bit insn, 24 bit disp -> 26 bit range.  */
  {0x2000000 - 1 - 2, -0x2000000 - 2, 2, 0 },
  /* Same thing, but with leading nop for alignment.  */
  {0x2000000 - 1 - 2, -0x2000000 - 2, 4, 0 }
};

/* Return an initial guess of the length by which a fragment must grow to
   hold a branch to reach its destination.
   Also updates fr_type/fr_subtype as necessary.

   Called just before doing relaxation.
   Any symbol that is now undefined will not become defined.
   The guess for fr_var is ACTUALLY the growth beyond fr_fix.
   Whatever we do to grow fr_fix or fr_var contributes to our returned value.
   Although it may not be explicit in the frag, pretend fr_var starts with a
   0 value.  */

int
md_estimate_size_before_relax (fragS * fragP, segT segment)
{
  UNUSED(fragP);
  UNUSED(segment);
#if 0
  /* The only thing we have to handle here are symbols outside of the
     current segment.  They may be undefined or in a different segment in
     which case linker scripts may place them anywhere.
     However, we can't finish the fragment here and emit the reloc as insn
     alignment requirements may move the insn about.  */

  if (S_GET_SEGMENT (fragP->fr_symbol) != segment)
    {
      /* The symbol is undefined in this segment.
	 Change the relaxation subtype to the max allowable and leave
	 all further handling to md_convert_frag.  */
      fragP->fr_subtype = 2;

      {
	const CGEN_INSN * insn;
	int               i;

	/* Update the recorded insn.
	   Fortunately we don't have to look very far.
	   FIXME: Change this to record in the instruction the next higher
	   relaxable insn to use.  */
	for (i = 0, insn = fragP->fr_cgen.insn; i < 4; i++, insn++)
	  {
	    if ((strcmp (CGEN_INSN_MNEMONIC (insn),
			 CGEN_INSN_MNEMONIC (fragP->fr_cgen.insn))
		 == 0)
		&& CGEN_INSN_ATTR_VALUE (insn, CGEN_INSN_RELAXED))
	      break;
	  }
	if (i == 4)
	  assert(0);

	fragP->fr_cgen.insn = insn;
	return 2;
      }
    }

  return md_relax_table[fragP->fr_subtype].rlx_length;
#endif
  return 0;
}

/* *fragP has been relaxed to its final size, and now needs to have
   the bytes inside it modified to conform to the new size.

   Called after relaxation is finished.
   fragP->fr_type == rs_machine_dependent.
   fragP->fr_subtype is the subtype of what the address relaxed to.  */

void
md_convert_frag (bfd *   abfd ATTRIBUTE_UNUSED,
		 segT    sec  ATTRIBUTE_UNUSED,
		 fragS * fragP ATTRIBUTE_UNUSED)
{
  /* FIXME */
}


/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from_section (fixS * fixP, segT sec)
{
  if (fixP->fx_addsy != (symbolS *) NULL
      && (! S_IS_DEFINED (fixP->fx_addsy)
	  || S_GET_SEGMENT (fixP->fx_addsy) != sec))
    /* The symbol is undefined (or is defined but not in this section).
       Let the linker figure it out.  */
    return 0;

  return (fixP->fx_frag->fr_address + fixP->fx_where) & ~1;
}




/* Write a value out to the object file, using the appropriate endianness.  */

void
md_number_to_chars (char * buf, valueT val, int n)
{
  number_to_chars_littleendian (buf, val, n);
}

/* Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message is returned, or NULL on OK.
*/

/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

char *
md_atof (int type, char * litP, int *  sizeP)
{
  return ieee_md_atof (type, litP, sizeP, TRUE);
}

bfd_boolean
vc4_fix_adjustable (fixS * fixP)
{
  /* We need the symbol name for the VTABLE entries.  */
  if (fixP->fx_r_type == BFD_RELOC_VTABLE_INHERIT
      || fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    return 0;

  return 1;
}

/* GAS will call this for each fixup.  It should store the correct
   value in the object file.  */

void
md_apply_fix (fixS *fixP, valueT * valP, segT seg)
{
  printf("md_apply_fix %d %s 0x%lx %p %p %d\n",
	 fixP->fx_r_type, ""/*bfd_reloc_code_real_names[fixP->fx_r_type]*/,
	 fixP->fx_where,
	 fixP->fx_addsy, fixP->fx_subsy, fixP->fx_pcrel);

  unsigned char *where;
  uint16_t ins[5];
  long value = *valP;

  if (fixP->fx_addsy == (symbolS *) NULL)
    fixP->fx_done = 1;

  else if (fixP->fx_pcrel)
    {
      segT s = S_GET_SEGMENT (fixP->fx_addsy);

      if (s == seg || s == absolute_section)
	{
	  value += S_GET_VALUE (fixP->fx_addsy);
	  fixP->fx_done = 1;
	}
    }

  /* We don't actually support subtracting a symbol.  */
  if (fixP->fx_subsy != (symbolS *) NULL)
    as_bad_where (fixP->fx_file, fixP->fx_line, _("expression too complex"));

  switch (fixP->fx_r_type)
    {
    default:
      fixP->fx_no_overflow = 1;
      break;
    case BFD_RELOC_AVR_7_PCREL:
    case BFD_RELOC_AVR_13_PCREL:
    case BFD_RELOC_32:
    case BFD_RELOC_16:
    case BFD_RELOC_AVR_CALL:
      break;
    }

  if (fixP->fx_done)
    {
      size_t len, i;

      /* Fetch the instruction, insert the fully resolved operand
	 value, and stuff the instruction back again.  */
      where = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;

      switch (fixP->fx_r_type)
	{
	case BFD_RELOC_VC4_REL7_MUL2:
	case BFD_RELOC_VC4_REL8_MUL2:
	case BFD_RELOC_VC4_REL10_MUL2:
	case BFD_RELOC_VC4_REL16:
	case BFD_RELOC_VC4_REL23_MUL2:
	case BFD_RELOC_VC4_REL27:
	case BFD_RELOC_VC4_REL27_MUL2:
	case BFD_RELOC_VC4_REL32:
	case BFD_RELOC_VC4_IMM5_1:
	case BFD_RELOC_VC4_IMM5_2:
	case BFD_RELOC_VC4_IMM6_MUL4:
	case BFD_RELOC_VC4_IMM11:
	case BFD_RELOC_VC4_IMM12:
	case BFD_RELOC_VC4_IMM16:
      /*case BFD_RELOC_VC4_IMM23:*/
	case BFD_RELOC_VC4_IMM27:
	case BFD_RELOC_VC4_IMM32:
	case BFD_RELOC_VC4_IMM32_2:

	  len = vc4_bfd_fixup_get_length(fixP->fx_r_type);

	  for (i=0; i<len; i++) {
	    ins[i] = bfd_getl16(where + i * 2);
	  }

	  switch (vc4_bfd_fixup_get_divide(fixP->fx_r_type)) {
	  case 1:
	    break;
	  case 2:
	    if (value & 1)
	      as_bad_where(fixP->fx_file, fixP->fx_line,
			   _("address operand not divisible by 2: %ld"), value);
	    value >>= 1;
	    break;
	  case 4:
	    if (value & 3)
	      as_bad_where(fixP->fx_file, fixP->fx_line,
			   _("address operand not divisible by 4: %ld"), value);
	    value >>= 2;
	    break;
	  default:
	    assert(0);
	  }

	  vc4_bfd_fixup_set(fixP->fx_r_type, ins, value);

	  for (i=0; i<len; i++) {
	    bfd_putl16(ins[i], where + i * 2);
	  }
	  break;

        default:
	  as_fatal (_("line %d: unknown relocation type: 0x%x (%u)"),
		    fixP->fx_line, fixP->fx_r_type, fixP->fx_r_type);
	  break;
	}
    }
  else
    {
      switch ((int) fixP->fx_r_type)
	{
#if 0
	case -BFD_RELOC_AVR_HI8_LDI_NEG:
	case -BFD_RELOC_AVR_HI8_LDI:
	case -BFD_RELOC_AVR_LO8_LDI_NEG:
	case -BFD_RELOC_AVR_LO8_LDI:
	  as_bad_where (fixP->fx_file, fixP->fx_line,
			_("only constant expression allowed"));
	  fixP->fx_done = 1;
	  break;
#endif
	default:
	  break;
	}
    }
}

static const char *get_name(symbolS *s)
{
  return (s && S_GET_NAME(s)) ? S_GET_NAME(s) : "";
}

arelent *
vc4_tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED,
		  fixS *fixp)
{
  arelent *reloc;

  printf("%s: %p %p:%s %p:%s\n", __func__,
	 fixp,
	 fixp->fx_addsy, get_name(fixp->fx_addsy),
	 fixp->fx_subsy, get_name(fixp->fx_subsy));

  if (fixp->fx_addsy && fixp->fx_subsy)
    {
      long value = 0;

      if ((S_GET_SEGMENT (fixp->fx_addsy) != S_GET_SEGMENT (fixp->fx_subsy))
          || S_GET_SEGMENT (fixp->fx_addsy) == undefined_section)
        {
          as_bad_where (fixp->fx_file, fixp->fx_line,
              "Difference of symbols in different sections is not supported");
          return NULL;
        }

      /* We are dealing with two symbols defined in the same section.
         Let us fix-up them here.  */
      value += S_GET_VALUE (fixp->fx_addsy);
      value -= S_GET_VALUE (fixp->fx_subsy);

      /* When fx_addsy and fx_subsy both are zero, md_apply_fix
         only takes it's second operands for the fixup value.  */
      fixp->fx_addsy = NULL;
      fixp->fx_subsy = NULL;
      md_apply_fix (fixp, (valueT *) &value, NULL);

      return NULL;
    }

  reloc = xmalloc (sizeof (arelent));

  reloc->sym_ptr_ptr = xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);

  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    _("reloc %d not supported by object file format"),
		    (int) fixp->fx_r_type);
      return NULL;
    }

  if (fixp->fx_r_type == BFD_RELOC_VTABLE_INHERIT
      || fixp->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    reloc->address = fixp->fx_offset;

  reloc->addend = fixp->fx_offset;

  return reloc;
}
