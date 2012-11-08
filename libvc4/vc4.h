#ifndef VC4_H__
#define VC4_H__

#include <inttypes.h>

uint16_t get_le16(const uint8_t *b);

struct vc4_decode_table
{
	struct vc4_decode_table *next;

	char code;
	size_t count;
	char tab[32][16];
};

#define VC4_PX_LIST(m) \
	VC4_PX(m, (reg_0_15, "r%d", (num))) \
	VC4_PX(m, (reg_0_31, "r%d", (num))) \
	VC4_PX(m, (reg_0_6_16_24, "r%d", ([\x00\x06\x10\x18]num))) \
	VC4_PX(m, (reg_r6, "r6", ())) \
	VC4_PX(m, (reg_sp, "sp", ())) /* 25 */ \
	VC4_PX(m, (reg_lr, "lr", ())) /* 26 */ \
	VC4_PX(m, (reg_sr, "sr", ())) /* 30 */ \
	VC4_PX(m, (reg_pc,       "pc", ())) /* 31 */ \
	VC4_PX(m, (reg_cpuid,    "cpuid", ())) \
	VC4_PX(m, (reg_range,    "r%d-r%d", ())) \
	VC4_PX(m, (reg_range_r6, "r6-r%d", ())) \
	VC4_PX(m, (reg_shl, "", ()))		\
	VC4_PX(m, (reg_shl_8, "", ())) \
	VC4_PX(m, (reg_shl_p1, "", ())) \
	VC4_PX(m, (num_u_shl_p1, "", ())) \
	VC4_PX(m, (num_s_shl_p1, "", ())) \
 \
	VC4_PX(m, (num_u, "%u", (num)))	/* unsigned int             #0x%04x{u} */ \
	VC4_PX(m, (num_s, "%u", (num)))		/* signed int               #0x%04x{i} */ \
	VC4_PX(m, (num_u4, "%u", (num*4)))		/* unsigned int             #0x%04x{u*4} */ \
	VC4_PX(m, (num_s4, "%s", (num*4)))		/* signed int               #0x%04x{i*4} */ \
	VC4_PX(m, (addr_reg, "(r%d)", (num)))	        /* addr of reg              (r%i{s}) */ \
	VC4_PX(m, (addr_reg_num_u, "%u(r%d)", (num)))	/* addr of reg + unsigned   0x%04x{u}(r%i{s}) */ \
	VC4_PX(m, (addr_reg_num_s, "%d(r%d)", (num)))	/* addr of reg + signed     0x%04x{i}(r%i{s}) */ \
	VC4_PX(m, (addr_reg_post_inc, "(r%d)++", (num)))/* addr of reg              (r%i{s})++ */ \
	VC4_PX(m, (addr_reg_pre_dec, "--(r%d)", (num)))	/* addr of reg              --(r%i{s}) */ \
 \
	VC4_PX(m, (pc_rel_s,   "$+%u", (num)))		/* pc rel addr + signed     0x%08x{$+o} */ \
	VC4_PX(m, (pc_rel_s2,  "$+%u", (num*2)))	/* pc rel addr + signed     0x%08x{$+o*2} */ \
	VC4_PX(m, (pc_rel_s4,  "$+%u", (num*4)))	/* pc rel addr + signed     0x%08x{$+o*4} */ \
 \
	VC4_PX(m, (sp_rel_s,   "sp+%d", (num)))		/* sp rel addr + signed     0x%08x{sp+o} */ \
	VC4_PX(m, (sp_rel_s2,  "sp+%d", (num*2)))	/* sp rel addr + signed     0x%08x{sp+o*2} */ \
	VC4_PX(m, (sp_rel_s4,  "sp+%d", (num*4)))	/* sp rel addr + signed     0x%08x{sp+o*4} */ \
 \
	VC4_PX(m, (r24_rel_s,  "r24+%d", (num)))	/* r24 rel addr + signed    0x%08x{r24+o} */ \
	VC4_PX(m, (r24_rel_s2, "r24+%d", (num*2)))	/* r24 rel addr + signed    0x%08x{r24+o*2} */ \
	VC4_PX(m, (r24_rel_s4, "r24+%d", (num*4)))	/* r24 rel addr + signed    0x%08x{r24+o*4} */ \
 \
	VC4_PX(m, (r0_rel_s,   "r0+%d", (num)))		/* r0 rel addr + signed    0x%08x{r0+o} */ \
	VC4_PX(m, (r0_rel_s2,  "r0+%d", (num*2)))	/* r0 rel addr + signed    0x%08x{r0+o*2} */ \
	VC4_PX(m, (r0_rel_s4,  "r0+%d", (num*4)))	/* r0 rel addr + signed    0x%08x{r0+o*4} */ \


	/*
	VC4_PX(m, (num_u4_mul4))
	VC4_PX(m, (addr_sp, ""))
	VC4_PX(m, (addr_sp_offset_mul4))
	VC4_PX(m, (num_s6_mul4))
	VC4_PX(m, (num_s7_mul2
	*/

#define VC4_PX(a, b) VC4_PX_ ## a b

#define VC4_PX_ENUM(n, s, p) vc4_p_ ## n,
enum vc4_param_type
{
	vc4_p_unknown,

	VC4_PX_LIST(ENUM)

	vc4_p_MAX
};

struct vc4_param
{
	char *txt;
	enum vc4_param_type type;
	size_t reg_width;
	size_t num_width;
	char code;
	char code2;
};

struct vc4_val
{
	uint32_t value;
	uint32_t length;
};

struct vc4_opcode
{
	struct vc4_opcode *next;

	char string[81];
	char *format;
	size_t length;
	uint16_t mask, val;
	uint16_t mask2, val2;

	size_t num_params;
	struct vc4_param params[3];

	struct vc4_val vals[26];
};

/* Part of a 'pattern' opcode */
struct vc4_op_pat
{
	size_t count;
	struct {
		char code;
		uint32_t val;
	} pat[3];
};

struct vc4_asm
{
	struct vc4_asm *next;
	struct vc4_asm *next_all;

	char *str;
	struct vc4_op_pat pat;

	struct vc4_opcode *op;

	uint16_t ins[2];
};

struct vc4_opcode_tab
{
	size_t count;
	struct vc4_opcode *tab[1];
};

#define opcode_tab_size(n) offsetof(struct vc4_opcode_tab, tab[n])

struct vc4_info
{
	struct vc4_decode_table *tables;

	char signed_ops[10];

	struct vc4_opcode_tab *opcodes[0x10000];

	struct vc4_opcode *all_opcodes;

	struct vc4_asm *all_asms;
	struct vc4_asm *all_asms_tail;
};

uint16_t vc4_get_le16(const uint8_t *b);

uint16_t vc4_get_instruction_length(uint16_t b0);

struct vc4_info *vc4_read_arch_file(const char *path);

void vc4_free_info(struct vc4_info *info);

char *vc4_display(const struct vc4_info *info, const struct vc4_opcode *op, uint32_t addr, const uint8_t *b, uint32_t len);

const struct vc4_opcode *vc4_get_opcode(const struct vc4_info *info, const uint8_t *b, size_t l);

void vc4_build_values(struct vc4_val *vals, const struct vc4_opcode *op, const uint8_t *b, uint32_t len);

void vc4_add_opcode_tab(struct vc4_opcode_tab **tabp, struct vc4_opcode *op);

void vc4_get_opcodes(struct vc4_info *info);

void vc4_strncat(char **dest, const char *src, int len);
void vc4_strcat(char **dest, const char *src);

void vc4_trim_space(char *p);


#endif
