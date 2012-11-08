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
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <assert.h>

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

static void
ignore_pseudo (int val ATTRIBUTE_UNUSED)
{
  discard_rest_of_line ();
}

const char vc4_comment_chars [] = ";#";

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  { "word",     cons,           4 },
  { "proc",     ignore_pseudo,  0 },
  { "endproc",  ignore_pseudo,  0 },
  { NULL, 	NULL, 		0 }
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
  /*char buf[256];*/

  if (vc4_info == NULL) {
    vc4_info = vc4_read_arch_file(
      "/home/marmar01/src/rpi/videocoreiv/videocoreiv.arch");
  }
  vc4_get_opcodes(vc4_info);

  vc4_hash = hash_new();

  for (a = vc4_info->all_asms; a != NULL; a = a->next_all) {

    /*printf("%s\n", dump_asm_name(a, buf));*/

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
    ot_num
  };

struct op_info
{
  enum op_type type;
  expressionS exp;
  size_t width;
  int num, num2;
};

static char *print_op_info(const struct op_info *inf, char *buf)
{
  char temp[16];

  switch (inf->type)
    {
    case ot_unknown:         strcpy(buf, "<unknown>"); break;
    case ot_cpuid:           strcpy(buf, "cpuid"); break;
    case ot_reg:             sprintf(buf, "r%d", inf->num); break;
    case ot_reg_range:       sprintf(buf, "r%d-r%d", inf->num, inf->num2); break;
    case ot_reg_addr:        sprintf(buf, "(r%d)", inf->num); break;
    case ot_reg_addr_pi:     sprintf(buf, "(r%d)++", inf->num); break;
    case ot_reg_addr_pd:     sprintf(buf, "--(r%d)", inf->num); break;

    case ot_reg_addr_offset:
    case ot_num:
      sprintf(buf, "%d (%p %p %u:0x%08x  width:%u)", inf->exp.X_op,
	      inf->exp.X_add_symbol,
	      inf->exp.X_op_symbol,
	      (unsigned)inf->exp.X_add_number,
	      (unsigned)inf->exp.X_add_number,
	      inf->width);

      if (inf->type == ot_reg_addr_offset) {
	sprintf(temp, "(r%d)", inf->num);
	strcat(buf, temp);
      }
      break;

    default:
      printf("asdfasld999fk %s %d\n", __FUNCTION__, __LINE__);
      abort();
      break;
    }

  return buf;
}


static char *match_reg(char *str, int *num)
{
  int r;
  size_t l;
  int reg;

  if (str[0] == 'r' && isdigit(str[1])) {

    r = sscanf(str, "r%d%n", &reg, &l);

    if (r >= 1) {
      str += l;
      str = skip_space(str);
      *num = reg;
      return str;
    }
  }

  if (str[0] == 's' && str[1] == 'p' && !isalnum(str[2])) {
    *num = 25;
    str += 2;
    str = skip_space(str);
    return str;
  }
  if (str[0] == 'l' && str[1] == 'r' && !isalnum(str[2])) {
    *num = 26;
    str += 2;
    str = skip_space(str);
    return str;
  }
  if (str[0] == 's' && str[1] == 'r' && !isalnum(str[2])) {
    *num = 30;
    str += 2;
    str = skip_space(str);
    return str;
  }
  if (str[0] == 'p' && str[1] == 'c' && !isalnum(str[2])) {
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
  /*
  if (v & 0x80000000)
    return 1 + vc4_log2(~v);
  if (v & 0xffff0000)
    return 16 + vc4_log2(v >> 16);
  if (v & 0xff00)
    return 8 + vc4_log2(v >> 8);
  if (v & 0xf0)
    return 4 + vc4_log2(v >> 4);
  if (v & 0xc)
    return 2 + vc4_log2(v >> 2);
  if (v & 0x2)
    return 1 + vc4_log2(v >> 1);
  return 1;
  */
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
      inf->num = reg;
      return str;
    }
    else if (*str == '-') {
      str++;
      str = skip_space(str);

      cont = match_reg(str, &reg2);

      if (cont != NULL) {
	str = cont;

	inf->type = ot_reg_range;
	inf->num = reg;
	inf->num2 = reg2;
	return str;
      }
    }
    else {
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
	  inf->num = reg;
	  return str;
	}
      }
    }
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
	    inf->num = reg;
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
  const int max_ops = 3;
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

#define VC4_PX_NAME(n, s, p) # n,
static const char *const vc4_param_type_name[] =
  {
    "unknown",
    VC4_PX_LIST(NAME)
    "MAX" 
  };

static int match_op_info_to_vc4_asm_item(struct vc4_param *param, struct op_info *ops)
{
  printf("match %d[%s] = %d %d %u\n", param->type, vc4_param_type_name[param->type], ops->type, ops->num, ops->width);

  switch (param->type)
    {
    case vc4_p_unknown:
      printf("UNKNOWN!!\n");
      break;

    case vc4_p_num_u:
    case vc4_p_num_s:
      if (ops->type == ot_num) {
	if (ops->width <= param->num_width)
	  return 0;
	return ops->width - param->num_width;
      }
      break;

    case vc4_p_addr_reg:
      if (ops->type == ot_reg_addr)
	return 0;
      break;

    case vc4_p_addr_reg_post_inc:
      if (ops->type == ot_reg_addr_pi)
	return 0;
      break;

    case vc4_p_addr_reg_pre_dec:
      if (ops->type == ot_reg_addr_pd)
	return 0;
      break;

    case vc4_p_addr_reg_num_u:
    case vc4_p_addr_reg_num_s:
      if (ops->type == ot_reg_addr_offset) {
	if (ops->width <= param->num_width)
	  return 0;
	return ops->width - param->num_width;
      }
      break;

    case vc4_p_pc_rel_s:
    case vc4_p_pc_rel_s2:
    case vc4_p_pc_rel_s4:
      if (ops->type == ot_num) {
	if (ops->width <= param->num_width)
	  return 0;
	return ops->width - param->num_width;
      }
      break;

    case vc4_p_sp_rel_s:
    case vc4_p_sp_rel_s2:
    case vc4_p_sp_rel_s4:
      /* TODO */
      break;

    case vc4_p_r0_rel_s:
    case vc4_p_r0_rel_s2:
    case vc4_p_r0_rel_s4:
      /* TODO */
      break;

    case vc4_p_r24_rel_s:
    case vc4_p_r24_rel_s2:
    case vc4_p_r24_rel_s4:
      /* TODO */
      break;

    case vc4_p_reg_0_31:
      if (ops->type == ot_reg)
	return 0;
      break;

    case vc4_p_reg_0_15:
      if (ops->type == ot_reg && ops->num < 16)
	return 0;
      break;

    case vc4_p_reg_0_6_16_24:
      if (ops->type == ot_reg &&
	  (ops->num == 0 || ops->num == 6 || ops->num == 16 || ops->num == 24))
	return 0;
      break;

    case vc4_p_reg_r6:
      if (ops->type == ot_reg && ops->num == 6)
	return 0;
      break;

    case vc4_p_reg_sp:
      if (ops->type == ot_reg && ops->num == 25)
	return 0;
      break;

    case vc4_p_reg_lr:
      if (ops->type == ot_reg && ops->num == 26)
	return 0;
      break;

    case vc4_p_reg_sr:
      if (ops->type == ot_reg && ops->num == 30)
	return 0;
      break;

    case vc4_p_reg_pc:
      if (ops->type == ot_reg && ops->num == 31)
	return 0;
      break;

    case vc4_p_reg_range:
      if (ops->type == ot_reg_range)
	return 0;
      break;

    case vc4_p_reg_range_r6:
      if (ops->type == ot_reg_range && ops->num == 6)
	return 0;
      break;

    case vc4_p_reg_cpuid:
      if (ops->type == ot_cpuid)
	return 0;
      break;

    default:
      printf("asjdhfljaksdhflkashdfkahdsfklaj\n");
      /*abort();*/
      break;
  }

  return -1;
}


static struct vc4_asm *match_op_info_to_vc4_asm(size_t count, struct op_info *ops, struct vc4_asm *list)
{
  struct vc4_asm *opcode;
  struct vc4_asm *best;
  int best_error;
  int this_error;
  int ret;
  size_t i;
  int ers[3];

  best = NULL;
  best_error = INT_MAX;

  for (opcode = list; opcode != NULL; opcode = opcode->next) {

    if (opcode->op->num_params != count)
      continue;

    ers[0] = ers[1] = ers[2] = 0;

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

    printf("opcode = %-10s %s\n", opcode->str, opcode->op->format);
    {
      printf("  ");
      for (i=0; i<opcode->pat.count; i++) {
	printf(" (%c/%u)",
	       opcode->pat.pat[i].code,
	       opcode->pat.pat[i].val);
      }
      for (i=0; i<opcode->op->num_params; i++) {
	printf(" %d[%u/%u/%c/%c]=%d", opcode->op->params[i].type,
	       opcode->op->params[i].reg_width,
	       opcode->op->params[i].num_width,
	       opcode->op->params[i].code ? opcode->op->params[i].code : ' ',
	       opcode->op->params[i].code2 ? opcode->op->params[i].code2 : ' ',
	       ers[i]);
      }
      printf(" => %d\n", this_error);
    }

    if (i == count && this_error >= 0) {
      if (this_error < best_error) {
	best_error = this_error;
	best = opcode;
      }
    }
  }

  return best;
}

static void fill_value(uint16_t *ins, const struct vc4_opcode *op, char code, uint32_t val)
{
  uint16_t mask;
  uint16_t *p;
  const char *f;

  printf("Fill %s %c %u %d\n", op->string, code, val, op->vals[code - 'a'].length);

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

static int is_param_pcrel(enum vc4_param_type type)
{
  switch (type) {
  case vc4_p_pc_rel_s:
  case vc4_p_pc_rel_s2:
  case vc4_p_pc_rel_s4:
    return 1;
    break;

  default:
    break;
  }
  return 0;
}

void
md_assemble (char * str)
{
  char op[11];
  struct vc4_asm *opcode;
  struct vc4_asm *list;
  size_t i;

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
	printf(" %d[%u/%u/%c/%c]", opcode->op->params[i].type,
	       opcode->op->params[i].reg_width,
	       opcode->op->params[i].num_width,
	       opcode->op->params[i].code ? opcode->op->params[i].code : ' ',
	       opcode->op->params[i].code2 ? opcode->op->params[i].code2 : ' ');
      }
      printf("\n");
    }
  }
  */

  dwarf2_emit_insn (0);

  {
    char *t = input_line_pointer;
    struct op_info ops[3];
    int count;

    memset(ops, 0x55, sizeof(ops));

    count = vc4_operands(&str, ops);
    if (*skip_space (str) || count < 0) {
      as_bad (_("garbage at end of line"));
      printf("[%s/%s] %d\n", str, skip_space (str), count);
    }
    input_line_pointer = t;

    opcode = match_op_info_to_vc4_asm(count, ops, list);

    if (opcode != NULL) {
      uint16_t ins[5];
      char buf[256];

      printf(">> opcode = %-10s %s (%s) %s\n",
	     opcode->str, opcode->op->format,
	     opcode->op->string, dump_asm_name(opcode, buf));

      char *frag = frag_more (opcode->op->length * 2);
      int where = frag - frag_now->fr_literal;
      char buf2[256];
      int x;

      ins[0] = opcode->ins[0];
      ins[1] = opcode->ins[1];
      ins[2] = ins[3] = ins[4] = 0;

      for (i=0; i<opcode->op->num_params; i++) {
	if (!opcode->op->params[i].code) {
	  continue;
	  /* This is normally OK, it means that the operand was a
	     constant, like r6 */
	}

	switch (ops[i].type) {
	case ot_num:
	case ot_reg_addr_offset:
	  if (ops[i].type == ot_reg_addr_offset) {
	    switch (opcode->op->params[i].type) {
	    case vc4_p_addr_reg_num_s:
	    case vc4_p_addr_reg_num_u:
	      fill_value(ins, opcode->op, opcode->op->params[i].code, ops[i].num);
	      break;
	      
	    default:
	      printf("asdfasld999fk\n");
	      break;
	    }
	  }

	  if (ops[i].exp.X_op == O_symbol) {
	    if (is_param_pcrel(opcode->op->params[i].type)) {
	      if (opcode->op->params[i].num_width == 23) {
		printf("fix_new_exp pc-rel 23 %s\n", print_op_info(&ops[i], buf2));
		fix_new_exp(frag_now, where, opcode->op->length * 2,
			    &ops[i].exp, TRUE, BFD_RELOC_VC4_REL23_MUL2);
	      }
	      else if (opcode->op->params[i].num_width == 27) {
		printf("fix_new_exp pc-rel 27 %s\n", print_op_info(&ops[i], buf2));
		fix_new_exp(frag_now, where, opcode->op->length * 2,
			    &ops[i].exp, TRUE, BFD_RELOC_VC4_REL27_MUL2);
	      }
	      else if (opcode->op->params[i].num_width == 32) {
		printf("fix_new_exp pc-rel 32 %s\n", print_op_info(&ops[i], buf2));
		fix_new_exp(frag_now, where, opcode->op->length * 2,
			    &ops[i].exp, TRUE, BFD_RELOC_VC4_REL32);
	      }
	    } else {
	      if (opcode->op->params[i].num_width == 23) {
		printf("fix_new_exp imm 23 %s\n", print_op_info(&ops[i], buf2));
		fix_new_exp(frag_now, where, opcode->op->length * 2,
			    &ops[i].exp, FALSE, BFD_RELOC_VC4_IMM23);
	      }
	      else if (opcode->op->params[i].num_width == 27) {
		printf("fix_new_exp imm 27 %s %x\n", print_op_info(&ops[i], buf2), where);
		fix_new_exp(frag_now, where, opcode->op->length * 2,
			    &ops[i].exp, FALSE, BFD_RELOC_VC4_IMM27);
	      }
	      else if (opcode->op->params[i].num_width == 32) {
		printf("fix_new_exp imm 32 %s %x\n", print_op_info(&ops[i], buf2), where);
		fix_new_exp(frag_now, where, opcode->op->length * 2,
			    &ops[i].exp, FALSE, BFD_RELOC_VC4_IMM32);
	      }
	    }
	  }
	  else if (ops[i].exp.X_op == O_constant) {
	    fill_value(ins, opcode->op, opcode->op->params[i].code, ops[i].exp.X_add_number);
	  }
	  break;

	case ot_cpuid:
	  break;

	case ot_reg:
	case ot_reg_addr:
	case ot_reg_addr_pi:
	case ot_reg_addr_pd:
	  fill_value(ins, opcode->op, opcode->op->params[i].code, ops[i].num);
	  break;

	case ot_reg_range:
	  switch(ops[i].num) {
	  case 0: x = 0; break;
	  case 6: x = 1; break;
	  case 16: x = 2; break;
	  case 24: x = 3; break;
	  default: abort(); break;
	  }
	  fill_value(ins, opcode->op, opcode->op->params[i].code, x);
	  fill_value(ins, opcode->op, opcode->op->params[i].code2, (ops[i].num2 - ops[i].num) & 31);
	  break;

	default:
	      printf("asdfasld999fk\n");
	  abort();
	  fill_value(ins, opcode->op, opcode->op->params[i].code, ops[i].num);
	  break;
	}
      }

      for (i=0; i<opcode->op->length; i++) {
	printf("%04x\n", ins[i]);
	bfd_putl16 ((bfd_vma)ins[i], frag + i * 2);
      }
    }
    else {
      as_bad (_("can't match operands to opcode"));
    }
  }
}


/* The syntax in the manual says constants begin with '#'.
   We just ignore it.  */

void
md_operand (expressionS * expressionP)
{
  UNUSED(expressionP);
#if 0
  if (* input_line_pointer == '#')
    {
      input_line_pointer ++;
      expression (expressionP);
    }
#endif
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
	  abort ();

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
  printf("md_apply_fix %d %s 0x%lx %p %d\n",
	 fixP->fx_r_type, ""/*bfd_reloc_code_real_names[fixP->fx_r_type]*/,
	 fixP->fx_where,
	 fixP->fx_addsy, fixP->fx_pcrel);

  unsigned char *where;
  unsigned long insn;
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
      /* Fetch the instruction, insert the fully resolved operand
	 value, and stuff the instruction back again.  */
      where = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
      insn = bfd_getl16 (where);

      switch (fixP->fx_r_type)
	{
	case BFD_RELOC_VC4_REL23_MUL2:
	  printf("md_apply_fix %d %s\n", fixP->fx_r_type, "BFD_RELOC_VC4_REL23_MUL2");

	  if (value & 1)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("odd address operand: %ld"), value);

	  /* Instruction addresses are always right-shifted by 1.  */
	  value >>= 1;

	  /*	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  */
	  bfd_putl16 ((bfd_vma) (insn | ((value >> 16) & 0x007f)), where);
	  bfd_putl16 ((bfd_vma) (value & 0xffff), where + 2);
	  break;

	case BFD_RELOC_VC4_REL27:
	  printf("md_apply_fix %d %s\n", fixP->fx_r_type, "BFD_RELOC_VC4_REL27");

	  /*	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  */
	  insn = bfd_getl16 (where + 2);
	  insn = (insn & 0xf800) | ((value >> 16) & 0x7ff);
	  bfd_putl16 ((bfd_vma) ((insn & 0xf800) | ((value >> 16) & 0x7ff)), where + 2);
	  bfd_putl16 ((bfd_vma) (value & 0xffff), where + 4);
	  break;

	case BFD_RELOC_VC4_REL27_MUL2:
	  printf("md_apply_fix %d %s\n", fixP->fx_r_type, "BFD_RELOC_VC4_REL27_MUL2");

	  if (value & 1)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("odd address operand: %ld"), value);

	  /* Instruction addresses are always right-shifted by 1.  */
	  value >>= 1;

	  /*	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  */
	  insn = bfd_getl16 (where);
	  insn = (insn & 0xf080);
	  insn |= (value >> 16) & 0x007f;
	  insn |= (value >> 15) & 0x0f00;
	  bfd_putl16 ((bfd_vma) (insn), where);
	  bfd_putl16 ((bfd_vma) (value & 0xffff), where + 2);
	  break;

	case BFD_RELOC_VC4_REL32:
	  printf("md_apply_fix %d %s\n", fixP->fx_r_type, "BFD_RELOC_VC4_REL32");

	  /*	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  */
	  bfd_putl16 ((bfd_vma) (value & 0xffff), where + 2);
	  bfd_putl16 ((bfd_vma) ((value >> 16) & 0xffff), where + 4);
	  break;

	case BFD_RELOC_VC4_IMM23:
	  printf("md_apply_fix %d %s\n", fixP->fx_r_type, "BFD_RELOC_VC4_IMM23");

	  /*	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  */
	  insn = bfd_getl16 (where);
	  bfd_putl16 ((bfd_vma) ((insn & 0xff80) | ((value >> 16) & 0x007f)), where);
	  bfd_putl16 ((bfd_vma) (value & 0xffff), where + 2);
	  break;

	case BFD_RELOC_VC4_IMM27:
	  printf("md_apply_fix %d %s\n", fixP->fx_r_type, "BFD_RELOC_VC4_IMM27");

	  /*	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  */
	  insn = bfd_getl16 (where + 2);
	  insn = (insn & 0xf800) | ((value >> 16) & 0x7ff);
	  bfd_putl16 ((bfd_vma) ((insn & 0xf800) | ((value >> 16) & 0x7ff)), where + 2);
	  bfd_putl16 ((bfd_vma) (value & 0xffff), where + 4);
	  break;

	case BFD_RELOC_VC4_IMM32:
	  printf("md_apply_fix %d %s\n", fixP->fx_r_type, "BFD_RELOC_VC4_IMM32");

	  /*	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  */
	  bfd_putl16 ((bfd_vma) (value & 0xffff), where + 2);
	  bfd_putl16 ((bfd_vma) ((value >> 16) & 0xffff), where + 4);
	  break;
#if 0
	case BFD_RELOC_AVR_13_PCREL:
	  if (value & 1)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("odd address operand: %ld"), value);

	  /* Instruction addresses are always right-shifted by 1.  */
	  value >>= 1;
	  --value;			/* Correct PC.  */

	  if (value < -2048 || value > 2047)
	    {
	      /* No wrap for devices with >8K of program memory.  */
	      if ((avr_mcu->isa & AVR_ISA_MEGA) || avr_opt.no_wrap)
		as_bad_where (fixP->fx_file, fixP->fx_line,
			      _("operand out of range: %ld"), value);
	    }

	  value &= 0xfff;
	  bfd_putl16 ((bfd_vma) (value | insn), where);
	  break;

	case BFD_RELOC_32:
	  bfd_putl32 ((bfd_vma) value, where);
	  break;

	case BFD_RELOC_16:
	  bfd_putl16 ((bfd_vma) value, where);
	  break;

	case BFD_RELOC_8:
          if (value > 255 || value < -128)
	    as_warn_where (fixP->fx_file, fixP->fx_line,
                           _("operand out of range: %ld"), value);
          *where = value;
	  break;

	case BFD_RELOC_AVR_16_PM:
	  bfd_putl16 ((bfd_vma) (value >> 1), where);
	  break;

	case BFD_RELOC_AVR_LDI:
	  if (value > 255)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value), where);
	  break;

	case BFD_RELOC_AVR_6:
	  if ((value > 63) || (value < 0))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  bfd_putl16 ((bfd_vma) insn | ((value & 7) | ((value & (3 << 3)) << 7) | ((value & (1 << 5)) << 8)), where);
	  break;

	case BFD_RELOC_AVR_6_ADIW:
	  if ((value > 63) || (value < 0))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  bfd_putl16 ((bfd_vma) insn | (value & 0xf) | ((value & 0x30) << 2), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 8), where);
	  break;

	case BFD_RELOC_AVR_MS8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 24), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 16), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 8), where);
	  break;

	case BFD_RELOC_AVR_MS8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 24), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 16), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI_PM:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 1), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI_PM:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 9), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI_PM:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 17), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI_PM_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 1), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI_PM_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 9), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI_PM_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 17), where);
	  break;

	case BFD_RELOC_AVR_CALL:
	  {
	    unsigned long x;

	    x = bfd_getl16 (where);
	    if (value & 1)
	      as_bad_where (fixP->fx_file, fixP->fx_line,
			    _("odd address operand: %ld"), value);
	    value >>= 1;
	    x |= ((value & 0x10000) | ((value << 3) & 0x1f00000)) >> 16;
	    bfd_putl16 ((bfd_vma) x, where);
	    bfd_putl16 ((bfd_vma) (value & 0xffff), where + 2);
	  }
	  break;

        case BFD_RELOC_AVR_8_LO:
          *where = 0xff & value;
          break;

        case BFD_RELOC_AVR_8_HI:
          *where = 0xff & (value >> 8);
          break;

        case BFD_RELOC_AVR_8_HLO:
          *where = 0xff & (value >> 16);
          break;
#endif
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

arelent *
vc4_tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED,
		  fixS *fixp)
{
  arelent *reloc;

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
