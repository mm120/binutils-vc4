/* tc-vc4.c -- Assembler for the Vc4 family.
   Copyright 2012
   Free Software Foundation.
   Contributed by Mark Marshall, markmarshall14@gmail.com

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
#include "safe-ctype.h"
#include <inttypes.h>
#include <limits.h>
#include <assert.h>

#include "elf/vc4.h"
#include "opcode/vc4.h"

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

static const char *get_name(symbolS *s)
{
  const char *n;
  if (s == NULL)
    return "";
  n = S_GET_NAME(s);
  if (n == NULL)
    return "";
  return n;
}

static char *dump_uint16s(char *buf, const uint16_t *dat, size_t len)
{
  size_t i, o;

  for (i = 0, o = 0; i < len; i++) {
    o += sprintf(buf + o, " %04x", dat[i]);
  }

  return buf + 1;
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



static struct hash_control *vc4_hash;

char *dump_asm_name(const struct vc4_asm *a, char *buf);

char *dump_asm_name(const struct vc4_asm *a, char *buf)
{
  switch (a->pat.count) {
  case 0:
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
  default:
    assert(0);
    break;
  }
  return buf;
}

void
md_begin (void)
{
  struct vc4_asm *a;

  vc4_load_opcode_info();

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

static char *dump_expression(const expressionS *exp, char *buf)
{
  if (exp == NULL) {
    strcpy(buf, "");
    return buf;
  }

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
	      exp->X_add_symbol, get_name(exp->X_add_symbol));
    } else {
      sprintf(buf, "{%s %p:%s %u:0x%08x}", "O_symbol",
	      exp->X_add_symbol, get_name(exp->X_add_symbol),
	      (unsigned)exp->X_add_number,
	      (unsigned)exp->X_add_number);
    }
    break;

  case O_subtract:
    if (exp->X_add_number == 0) {
      sprintf(buf, "{%s %p:%s %p:%s}", "O_subtract",
	      exp->X_add_symbol, get_name(exp->X_add_symbol),
	      exp->X_op_symbol, get_name(exp->X_op_symbol));
    } else {
      sprintf(buf, "{%s %p:%s %p:%s %u:0x%08x}", "O_subtract",
	      exp->X_add_symbol, get_name(exp->X_add_symbol),
	      exp->X_op_symbol, get_name(exp->X_op_symbol),
	      (unsigned)exp->X_add_number,
	      (unsigned)exp->X_add_number);
    }
    break;

  default:
    sprintf(buf, "{%d %p:%s %p:%s %u:0x%08x}",
	    exp->X_op,
	    exp->X_add_symbol, get_name(exp->X_add_symbol),
	    exp->X_op_symbol, get_name(exp->X_op_symbol),
	    (unsigned)exp->X_add_number,
	    (unsigned)exp->X_add_number);
    break;
  }

  return buf;
}

static char *dump_op_info(const struct op_info *inf, char *buf)
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
	      inf->reg, dump_expression(&inf->exp, buf2));
      break;

    case ot_reg_lsr:
      sprintf(buf, "r%d lsr %s",
	      inf->reg, dump_expression(&inf->exp, buf2));
      break;

    case ot_reg_addr_offset:
      sprintf(buf, "%s (r%d)",
	      dump_expression(&inf->exp, buf2), inf->reg);
      break;

    case ot_num:
      sprintf(buf, "%s",
	      dump_expression(&inf->exp, buf2));
      break;

    default:
      as_fatal("Bad op_info->type %d", inf->type);
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

static char *vc4_get_operand(char *str, struct op_info *inf)
{
  int reg, reg2;
  int pre_dec = 0;
  int post_inc = 0;
  char *cont;
  /*char buf[256];*/
  char *old_str;

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
    old_str = str;

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

    str = old_str;
  }

  if (strcmp(str, "cpuid") == 0 && !ISALNUM(str[5])) {
    str += 5;
    inf->type = ot_cpuid;
    return str;
  }

  input_line_pointer = str;
  expression/*_and_evaluate*/(&inf->exp);
  /*printf("E %s  <%s> [%s]\n", dump_expression(&inf->exp, buf), str, input_line_pointer);*/
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
	    if (inf->exp.X_op == O_constant &&
		inf->exp.X_add_number == 0) {
	      inf->type = ot_reg_addr;
	    } else {
	      inf->type = ot_reg_addr_offset;
	    }
	    inf->reg = reg;
	    return str;
	  }
	}
      }
    } else if (*str == ',' || *str == 0) {
      inf->type = ot_num;
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
    DEBUG(OPS, "OP%d = %s\n", i+1, dump_op_info(&ops[i], buf));
    if (*str == 0) {
      *line = str;
      return i + 1;
    }
  }

  return -1;
}

struct match_operands {
  enum vc4_param_type pt;
  enum op_type ot;
  uint32_t reg_bitmap;
  int width_match;
};

static const struct match_operands match_ops_data[] =
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
  { vc4_p_r0_rel_s,             ot_reg_addr, 	 	(1u << 0), 0 },
  { vc4_p_r0_rel_s2,            ot_reg_addr, 	 	(1u << 0), 0 },
  { vc4_p_r0_rel_s4,            ot_reg_addr,  		(1u << 0), 0 },

  { vc4_p_r24_rel_s,            ot_reg_addr_offset, 	(1u << 24), 1 },
  { vc4_p_r24_rel_s2,           ot_reg_addr_offset, 	(1u << 24), 2 },
  { vc4_p_r24_rel_s4,           ot_reg_addr_offset, 	(1u << 24), 4 },
  { vc4_p_r24_rel_s,            ot_reg_addr,  		(1u << 24), 0 },
  { vc4_p_r24_rel_s2,           ot_reg_addr,  		(1u << 24), 0 },
  { vc4_p_r24_rel_s4,           ot_reg_addr,  		(1u << 24), 0 },

  { vc4_p_sp_rel_s,             ot_reg_addr_offset, 	(1u << 25), 1 },
  { vc4_p_sp_rel_s2,            ot_reg_addr_offset, 	(1u << 25), 2 },
  { vc4_p_sp_rel_s4,            ot_reg_addr_offset, 	(1u << 25), 4 },
  { vc4_p_sp_rel_s,             ot_reg_addr, 		(1u << 25), 0 },
  { vc4_p_sp_rel_s2,            ot_reg_addr, 		(1u << 25), 0 },
  { vc4_p_sp_rel_s4,            ot_reg_addr,	 	(1u << 25), 0 },

  { vc4_p_pc_rel_s,             ot_num, 		0, 1 },
  { vc4_p_pc_rel_s2,            ot_num, 		0, 2 },
  { vc4_p_pc_rel_s4,            ot_num, 		0, 4 },
  /*
  { vc4_p_pc_rel_s,             ot_reg_addr_offset, 	(1u << 31), 1 },
  { vc4_p_pc_rel_s2,            ot_reg_addr_offset, 	(1u << 31), 2 },
  { vc4_p_pc_rel_s4,            ot_reg_addr_offset, 	(1u << 31), 4 },
  */
};

static int vc4_param_fits(const struct vc4_param *param, signed long long *pval);

/*
 * returns:
 *  -1 Doesn't fit
 *   0 fits exactly
 *   1 might fit
 */
static int match_op_info_to_vc4_asm_item(const struct vc4_param *param,
					 const struct op_info *ops)
{
  size_t i;

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

      if (ops->exp.X_op == O_constant) {
	signed long long val = ops->exp.X_add_number;
	return vc4_param_fits(param, &val) ? 0 : -1;
      }

      return 1;
    }
  }

  return -1;
}

struct match_ops
{
  const struct vc4_asm *as;
  uint16_t ins[5];
  int ers[5];
  int score;
  int set;
  int broken;
  bfd_reloc_code_real_type bfd_fixup;
  size_t op_index;
  const struct vc4_param *param;
};

static int match_comp(const void *va, const void *vb)
{
  const struct match_ops *a = (struct match_ops *)va;
  const struct match_ops *b = (struct match_ops *)vb;

  if (a->as->op->length != b->as->op->length)
    return a->as->op->length - b->as->op->length;
  if (a->score == 0)
    return -1;
  if (b->score == 0)
    return 1;
  return 0;
}

#define MAX_MATCHES 10

static int match_op_info_to_vc4_asm(struct match_ops matches[MAX_MATCHES],
				    size_t count,
				    const struct op_info *ops,
				    const struct vc4_asm *list)
{
  const struct vc4_asm *opcode;
  int this_error;
  int ret;
  size_t i, j;
  char buf[2][256];
  int num;
  int dir;
  int best;

  num = 0;

  for (opcode = list; opcode != NULL; opcode = opcode->next) {

    if (opcode->op->num_params != count)
      continue;

    this_error = 0;
    for (i=0; i<count; i++) {
      ret = match_op_info_to_vc4_asm_item(&opcode->op->params[i], &ops[i]);
      if (ret < 0) {
	this_error = -1;
	break;
      }
      assert(num < MAX_MATCHES);
      matches[num].ers[i] = ret;
      this_error += ret;
    }

    if (this_error >= 0) {
      matches[num].as = opcode;
      matches[num].score = this_error;
      num++;
    }
  }

  assert(num <= MAX_MATCHES);

  qsort(matches, num, sizeof(matches[0]), match_comp);

  for (j = 0; j < (size_t)num; j++) {
    /* If we have a score of 0 it means that that entry in the table
       will definately work, we don't need any more entries. */
    if (matches[j].score == 0) {
      num = j + 1;
      break;
    }
  }

  /* Now we go through the table, and if we have two entries that are
     the same length, we look it see if it's worth getting rid of one
     of them. */
  if (num > 1)
    for (j = 0; j < (size_t)(num - 1); j++) {

      if (matches[j].as->op->length != matches[j+1].as->op->length)
	continue;

      dir = 0;
      for (i=0; i<count; i++) {
	best = 0;
	if (matches[j].ers[i] == 0 && matches[j+1].ers[i] != 0) {
	  best = 1;
	} else if (matches[j].ers[i] != 0 && matches[j+1].ers[i] == 0) {
	  best = -1;
	} else if (matches[j].ers[i] != 0 && matches[j+1].ers[i] != 0) {
	  best = matches[j+1].as->op->params[i].num_width - matches[j].as->op->params[i].num_width;
	}

	if (best > 0) {
	  if (dir == -1) {
	    dir = 0;
	    break;
	  }
	  dir = 1;
	} else if (best < 0) {
	  if (dir == 1) {
	    dir = 0;
	    break;
	  }
	  dir = -1;
	}
      }

      if (dir == 1) {
	memmove(&matches[j], &matches[j+1], sizeof(matches[0]) * (num - (j + 1)));
	num--;
	j--;
      } else if (dir == -1) {
	if ((j + 2u) < (size_t)num) {
	  memmove(&matches[j+1], &matches[j+2], sizeof(matches[0]) * (num - (j + 2)));
	}
	num--;
	j--;
      }
    }

  if (num > 1)
    for (j = 0; j < (size_t)num; j++) {

      opcode = matches[j].as;

      DEBUG(MATCH, "+opcode = %-10s %-38s %s",
	     opcode->str,
	     opcode->op->format,
	     opcode->op->string);

      DEBUG(MATCH, "  ");
      for (i=0; i<opcode->pat.count; i++) {
	DEBUG(MATCH, " (%c/%u)",
	      opcode->pat.pat[i].code,
	      opcode->pat.pat[i].val);
      }
      for (i=0; i<opcode->op->num_params; i++) {
	DEBUG(MATCH, " %s = %d",
	      vc4_param_print(&opcode->op->params[i], buf[0]),
	      matches[j].ers[i]);
      }
      DEBUG(MATCH, " => %d\n", matches[j].score);
    }

  return num;
}

static void fill_value(uint16_t *ins, const struct vc4_opcode *op,
		       char code, uint32_t val)
{
  DEBUG(FILL, "Fill %s %c %u %d\n", op->string,
	code, val, op->vals[code - 'a'].length);

  vc4_fill_value(ins, NULL, op, code, val);
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

static int vc4_param_fits(const struct vc4_param *param, signed long long *pval)
{
  signed long long val = *pval;
  signed long long v_min;
  signed long long v_max;
  int divide = vc4_param_divide(param->type);

  if (divide > 1) {
    if (val % divide) {
      return 0;
    }
    val /= divide;
  }

  if (param->num_width == 32) {
    // No overflow checking needed!
    *pval = val;
    return 1;
  }

  if (vc4_param_has_num(param->type) < 0) {
    
    // signed
    v_min = -(1LL << (param->num_width - 1));
    v_max = (1LL << (param->num_width - 1)) - 1;
    
  } else {
    
    // unsigned
    v_min = 0;
    v_max = (1LL << param->num_width) - 1;
  }

  if (val < v_min || val > v_max) {
    return 0;
  }

  *pval = val;
  return 1;
}

static void build_match(struct match_ops *match, const struct op_info *ops)
{
  size_t i;
  const struct vc4_asm *opcode;
  char buf[256];

  opcode = match->as;

  assert(opcode != NULL);

  DEBUG(MATCH, ">opcode = %-10s %s (%s) %s\n",
	opcode->str, opcode->op->format,
	opcode->op->string, dump_asm_name(opcode, buf));

  match->ins[0] = opcode->ins[0];
  match->ins[1] = opcode->ins[1];
  match->ins[2] = match->ins[3] = match->ins[4] = 0;

  match->set = 0;
  match->broken = 0;
  match->bfd_fixup = 0;
  match->op_index = 0;
  match->param = NULL;

  for (i=0; i<opcode->op->num_params; i++) {

    switch (opcode->op->params[i].type) {
    case vc4_p_reg_0_6_16_24:
      fill_value(match->ins, opcode->op, opcode->op->params[i].reg_code,
		 get_reg_div8(ops[i].reg));
      break;

    case vc4_p_reg_range:
      fill_value(match->ins, opcode->op, opcode->op->params[i].reg_code,
		 get_reg_div8(ops[i].reg));
      fill_value(match->ins, opcode->op, opcode->op->params[i].num_code,
		 (ops[i].num2 - ops[i].reg) & 31);
      break;

    case vc4_p_reg_range_r6:
      assert(ops[i].reg == 6);
      fill_value(match->ins, opcode->op, opcode->op->params[i].num_code,
		 (ops[i].num2 - ops[i].reg) & 31);
      break;

    case vc4_p_reg_shl:
    case vc4_p_reg_lsr:
      if (ops[i].exp.X_op == O_constant) {
	fill_value(match->ins, opcode->op, opcode->op->params[i].num_code,
		   ops[i].exp.X_add_number);
      } else {
	as_fatal("Need a constant for reg_shl / reg_lsr");
      }
      break;

    case vc4_p_reg_shl_p1:
    case vc4_p_reg_lsr_p1:
      if (ops[i].exp.X_op == O_constant) {
	fill_value(match->ins, opcode->op, opcode->op->params[i].num_code,
		   ops[i].exp.X_add_number - 1);
      } else {
	as_fatal("Need a constant for reg_shl / reg_lsr");
      }
      break;

    default:
      break;
    }

    if (vc4_param_has_reg(opcode->op->params[i].type)) {
      assert(ops[i].reg < (1 << opcode->op->params[i].reg_width));
      fill_value(match->ins, opcode->op, opcode->op->params[i].reg_code, ops[i].reg);
    }

    if (vc4_param_has_num(opcode->op->params[i].type) && (ops[i].type != ot_reg_addr)) {
      
      signed long long val;

      assert((ops[i].type == ot_reg_addr_offset) ||
	     (ops[i].type == ot_num));

      if (ops[i].exp.X_op == O_constant) {
	
	val = ops[i].exp.X_add_number;

	if (!vc4_param_fits(&opcode->op->params[i], &val))
	  match->broken = 1;
	
	DEBUG(OPS, "ON: %lld %zd %d\n",
	      (long long)ops[i].exp.X_add_number,
	      opcode->op->params[i].num_width,
	      match->broken);

	fill_value(match->ins, opcode->op, opcode->op->params[i].num_code, (uint32_t)val);

      } else {

	if (match->set) {
	  as_bad("Can't have more than one variable part of an instruction");
	  match->broken = 1;
	}

	match->set = 1;
	match->op_index = i;
	match->param = &opcode->op->params[i];
      }
    }
  }

  if (!match->broken && match->set) {

    match->bfd_fixup = vc4_bfd_fixup_get(opcode->op->string,
				  match->param->num_code,
				  vc4_param_pc_rel(match->param->type),
				  vc4_param_divide(match->param->type));

    if (match->bfd_fixup == 0) {
      as_bad("%s: Can't find bfd fixup type! %zd %s  %s %c %d %d\n", __func__,
	     match->param->num_width,
	     dump_op_info(&ops[match->op_index], buf),
	     opcode->op->string,
	     match->param->num_code,
	     vc4_param_pc_rel(match->param->type),
	     vc4_param_divide(match->param->type));
      match->broken = 1;
    }
  }
}

void
md_assemble (char * str)
{
  char op[16];
  const struct vc4_asm *opcode;
  const struct vc4_asm *list;
  size_t i;
  char *t;
  struct op_info ops[5];
  int count;
  char buf[256];
  struct match_ops matches[MAX_MATCHES];
  size_t num_matches;
  size_t match_index;

  DEBUG(BASIC, "A %s\n", str);

  str = skip_space(extract_word(str, op, sizeof(op)));

  if (!op[0]) {
    as_bad(_("can't find opcode '%s'"), str);
    return;
  }

  list = (const struct vc4_asm *) hash_find (vc4_hash, op);

  if (list == NULL) {
    as_bad(_("unknown opcode '%s'"), op);
    return;
  }

  t = input_line_pointer;

  memset(ops, 0, sizeof(ops));

  count = vc4_operands(&str, ops);
  if (count < 0) {
    as_bad(_("Can't parse operands"));
    return;
  }
  if (*skip_space(str)) {
    as_bad(_("garbage at end of line '%s'"), str);
    return;
  }
  input_line_pointer = t;

  num_matches = match_op_info_to_vc4_asm(matches, count, ops, list);

  if (num_matches == 0) {
    as_bad(_("can't match operands to opcode"));
    return;
  }

  for (match_index = 0; match_index < num_matches; match_index++) {
    build_match(&matches[match_index], ops);
  }

  for (match_index = 0; match_index < num_matches; ) {
    if (matches[match_index].broken) {
      DEBUG(MATCH, "X %zd %zd\n", match_index, num_matches);
      memmove(&matches[match_index], &matches[match_index+1], sizeof(matches[0]) * (num_matches - match_index));
      num_matches--;
    } else {
      match_index++;
    }
  }

  if (num_matches < 1) {
    as_bad("Failed to fully match an op-code");
    return;
  }

  /* We can't have more than three matches here! */
  assert(num_matches <= 3);

  dwarf2_emit_insn (0);

  char *frag;

  opcode = matches[0].as;

  if (num_matches > 1 && matches[0].set) {

    DEBUG(FRAG, "frag_var: %zd %p\n", opcode->op->length, frag_now);

    frag_now->tc_frag_data.num = num_matches;
    frag_now->tc_frag_data.cur = 0;
    frag_now->tc_frag_data.op_inf = ops[matches[0].op_index];

    for (i=0; i<num_matches; i++) {
      frag_now->tc_frag_data.d[i].as = matches[i].as;
      frag_now->tc_frag_data.d[i].param = matches[i].param;
      memcpy(&frag_now->tc_frag_data.d[i].ins, matches[i].ins, sizeof(uint16_t) * 5);
      frag_now->tc_frag_data.d[i].bfd_fixup = matches[i].bfd_fixup;
    }

    for (i=1; i<num_matches; i++) {
      if (matches[0].op_index != matches[i].op_index) {
	as_fatal("Two options don't have the same op_info??");
      }
    }

    frag = frag_var(rs_machine_dependent, 10,
		    frag_now->tc_frag_data.d[0].as->op->length * 2,
		    99,
		    frag_now->tc_frag_data.op_inf.exp.X_add_symbol,
		    frag_now->tc_frag_data.op_inf.exp.X_add_number,
		    (char *)opcode); /* fr_opcode */

    DEBUGn(FRAG, "frag_var %s %d %zd/%zd %zd %zd %s\n",
	   vc4_param_pc_rel(matches[0].param->type) ? "pc-rel" : "imm",
	   matches[0].bfd_fixup,
	   matches[0].param->num_width,
	   vc4_bfd_fixup_get_width(matches[0].bfd_fixup),
	   vc4_bfd_fixup_get_ins_length(matches[0].bfd_fixup),
	   vc4_bfd_fixup_get_divide(matches[0].bfd_fixup),
	   dump_op_info(&ops[matches[0].op_index], buf));

  } else {

    frag = frag_more(opcode->op->length * 2);

    if (matches[0].set) {

      int where = frag - frag_now->fr_literal;

      if (ops[matches[0].op_index].exp.X_op == O_symbol) {

	DEBUGn(FIX, "fix_new_exp %s %d %zd/%zd %zd %zd %s\n",
	       vc4_param_pc_rel(matches[0].param->type) ? "pc-rel" : "imm",
	       matches[0].bfd_fixup,
	       matches[0].param->num_width,
	       vc4_bfd_fixup_get_width(matches[0].bfd_fixup),
	       vc4_bfd_fixup_get_ins_length(matches[0].bfd_fixup),
	       vc4_bfd_fixup_get_divide(matches[0].bfd_fixup),
	       dump_op_info(&ops[matches[0].op_index], buf));
	
	fix_new_exp(frag_now, where,
		    opcode->op->length * 2,
		    &ops[matches[0].op_index].exp,
		    vc4_param_pc_rel(matches[0].param->type),
		    matches[0].bfd_fixup);
      } else {

	DEBUGn(FIX, "fixup_odd %s %d %zd/%zd %zd %zd %s\n",
	       vc4_param_pc_rel(matches[0].param->type) ? "pc-rel" : "imm",
	       matches[0].bfd_fixup,
	       matches[0].param->num_width,
	       vc4_bfd_fixup_get_width(matches[0].bfd_fixup),
	       vc4_bfd_fixup_get_ins_length(matches[0].bfd_fixup),
	       vc4_bfd_fixup_get_divide(matches[0].bfd_fixup),
	       dump_op_info(&ops[matches[0].op_index], buf));
      }
    }
  }

  assert(frag != NULL);

  for (i=0; i<opcode->op->length; i++) {
    bfd_putl16 ((bfd_vma)matches[0].ins[i], frag + i * 2);
  }

  DEBUG(BASIC, "%s\n", dump_uint16s(buf, matches[0].ins, opcode->op->length));
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

int vc4_relax_frag(asection *s, struct frag *fragP, long l)
{
  const struct vc4_asm *as = (const struct vc4_asm *)fragP->fr_opcode;

  DEBUGn(FRAG, "%p %p %lx | %x %x  %d %d %d  %s  <%s>\n", s, fragP, l,
	 (unsigned int)fragP->fr_address, (unsigned int)fragP->last_fr_address,
	 (int)fragP->fr_fix, (int)fragP->fr_var, (int)fragP->fr_offset,
	 get_name(fragP->fr_symbol), as->str);

  struct vc4_frag_option *fo;
  signed long long val;
  int oldsize;
  int newsize;

  oldsize = fragP->fr_var;

  if (S_IS_DEFINED(fragP->fr_symbol)) {

    for (;;) {
      fo = &fragP->tc_frag_data.d[fragP->tc_frag_data.cur];

      val = S_GET_VALUE(fragP->fr_symbol);
      val += fragP->fr_offset;
      if (vc4_param_pc_rel(fo->param->type))
	val -= fragP->fr_address;

      if (vc4_param_fits(fo->param, &val))
	break;

      if ((fragP->tc_frag_data.cur + 1) >= fragP->tc_frag_data.num) {
	as_bad("Cant find an option that fits!");
	break;
      }

      fragP->tc_frag_data.cur++;
    }

  } else {

    /* Assume the worst */
    fragP->tc_frag_data.cur = fragP->tc_frag_data.num - 1;

    fo = &fragP->tc_frag_data.d[fragP->tc_frag_data.cur];
  }
  newsize = fo->as->op->length * 2;

  fragP->fr_var = newsize;

  return newsize - oldsize;
}


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
  struct vc4_frag_option *fo;
  signed long long val;

  fragP->tc_frag_data.cur = 0;

  if (S_IS_DEFINED(fragP->fr_symbol)) {

    for (;;) {
      fo = &fragP->tc_frag_data.d[fragP->tc_frag_data.cur];

      val = S_GET_VALUE(fragP->fr_symbol);
      val += fragP->fr_offset;
      if (vc4_param_pc_rel(fo->param->type))
	val -= fragP->fr_address;

      if (vc4_param_fits(fo->param, &val))
	break;

      if ((fragP->tc_frag_data.cur + 1) >= fragP->tc_frag_data.num) {
	as_bad("Cant find an option that fits!");
	break;
      }

      fragP->tc_frag_data.cur++;
    }

  } else {

    /* Assume the worst */
    fragP->tc_frag_data.cur = fragP->tc_frag_data.num - 1;

    fo = &fragP->tc_frag_data.d[fragP->tc_frag_data.cur];
  }

  fragP->fr_var = fo->as->op->length * 2;
 
  DEBUGn(FRAG, "%p %lx %lld = %d\n", fragP, (long)segment, val, (int)fragP->fr_var);

  return fragP->fr_var;
}

void vc4_init_fix(fixS *f)
{
  DEBUGn(FIX, "%p %p:%s %p:%s  %d\n", f,
	 f->fx_addsy, get_name(f->fx_addsy),
	 f->fx_subsy, get_name(f->fx_subsy), f->fx_r_type);
}

void vc4_init_frag(fragS *f)
{
  DEBUGn(FRAG, "%p \n", f);
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
  /* 'buf' points to the start of the instruction, whether
     we need to change the instruction's fixed encoding.  */
  char *buf = fragP->fr_literal + fragP->fr_fix;
  struct vc4_frag_option *fo = &fragP->tc_frag_data.d[fragP->tc_frag_data.cur];
  size_t i;

  DEBUGn(FRAG, "frag %p bfd_type %d index %zd/%zd\n", fragP,
	 fo->bfd_fixup,
	 fragP->tc_frag_data.cur,
	 fragP->tc_frag_data.num);

  subseg_change (sec, 0);

  fixS *f = fix_new (fragP,
		     fragP->fr_fix,
		     fragP->fr_var,
		     fragP->fr_symbol,
		     fragP->fr_offset,
		     vc4_param_pc_rel(fo->param->type),
		     fo->bfd_fixup);

  buf = fragP->fr_literal + fragP->fr_fix;

  DEBUGn(FIX, "fix %p\n", f);

  for (i=0; i<fo->as->op->length; i++) {
    bfd_putl16 ((bfd_vma)fo->ins[i], buf + i * 2);
  }

  fragP->fr_fix += fragP->fr_var;
  fragP->fr_var = 0;
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
  gas_assert (fixP->fx_r_type <= BFD_RELOC_UNUSED);

  DEBUGn(FIX, "%d 0x%lx %p %p %d\n",
	 fixP->fx_r_type,
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
	case BFD_RELOC_VC4_IMM23:
	case BFD_RELOC_VC4_IMM27:
	case BFD_RELOC_VC4_IMM32:
	case BFD_RELOC_VC4_IMM32_2:

	  len = vc4_bfd_fixup_get_ins_length(fixP->fx_r_type);

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
	    as_fatal("vc4_bfd_fixup_get_divide not 1, 2, or 4! (%zd)",
		     vc4_bfd_fixup_get_divide(fixP->fx_r_type));
	  }

	  vc4_bfd_fixup_set(fixP->fx_r_type, ins, value);

	  for (i=0; i<len; i++) {
	    bfd_putl16(ins[i], where + i * 2);
	  }
	  break;

        default:
	  as_fatal(_("line %d: unknown relocation type: 0x%x (%u)"),
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

arelent *
vc4_tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED,
		  fixS *fixp)
{
  arelent *reloc;

  DEBUGn(FIX, "%p %p:%s %p:%s\n",
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
