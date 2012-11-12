#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>

#include "vc4.h"

uint16_t vc4_get_le16(const uint8_t *b)
{
	return (uint16_t)b[0] | ((uint16_t)b[1] << 8);
}

uint16_t vc4_get_instruction_length(uint16_t b0)
{
	if ((b0 & 0x8000) == 0)
		return 1;
	else if ((b0 & 0xc000) == 0x8000)
		return 2;
	else if ((b0 & 0xe000) == 0xc000)
		return 2;
	else if ((b0 & 0xf000) == 0xe000)
		return 3;
	else if ((b0 & 0xf800) == 0xf000)
		return 3;
	else if ((b0 & 0xf800) == 0xf800)
		return 5;
	else {
		/*throw "Bad instruction format, cant get length %x" % b0*/
		fprintf(stderr, "Can't get length of %04x\n", b0);
		abort();
	}
}

void vc4_trim_space(char *p)
{
	if (p == NULL)
		return;
	while (isblank(*p)) {
		memmove(p, p+1, strlen(p));
	}
	char *q;
	q = p + strlen(p);
	while (q > p  && isblank(q[-1])) {
		q[-1] = 0;
		q--;
	}
}

void vc4_strncat(char **dest, const char *src, int len)
{
	char *new_dest;
	int r;

	r = asprintf(&new_dest, "%s%.*s", *dest, len, src);

	assert(new_dest != NULL);
	assert(r >= 0);

	free(*dest);

	*dest = new_dest;
}

void vc4_strcat(char **dest, const char *src)
{
	vc4_strncat(dest, src, strlen(src));
}

void vc4_add_opcode_tab(struct vc4_opcode_tab **tabp, struct vc4_opcode *op)
{
	uint16_t j;
	struct vc4_opcode_tab *tab;

	if ((tab = *tabp) == NULL) {
		tab = (struct vc4_opcode_tab *)calloc(1, opcode_tab_size(1));
		tab->count = 1;
		tab->tab[0] = op;
	} else {
		for (j=0; j<tab->count; j++)
			if (tab->tab[j] == op)
				return;

		tab = (struct vc4_opcode_tab *)realloc(tab, opcode_tab_size(tab->count + 1));
		tab->tab[tab->count] = op;
		tab->count++;
	}

	*tabp = tab;
}

void vc4_fill_value(uint16_t *ins, uint16_t *ins_mask, const struct vc4_opcode *op,
		    char code, uint32_t val)
{
	uint16_t mask;
	const char *f;
	size_t pi;

	assert(op->length >= 1 && op->length <= 5);
	assert(strlen(op->string) == 16 * op->length);
	assert(code >= 'a' && code <= 'z');

	if (op->vals[code - 'a'].length == 32) {
		val = ((val >> 16) & 0xffff) | ((val & 0xffff) << 16);
	}

	mask = 0x0000;
	pi = op->length;
	f = op->string + 16 * op->length;

	assert(*f == 0);

	for (;;) {
		if (mask == 0) {
			if (pi == 0)
				break;
			mask = 0x0001;
			pi--;
		}

		if (*--f == code) {
			ins[pi] &= ~mask;
			if (val & 1)
				ins[pi] |= mask;
			if (ins_mask != NULL)
				ins_mask[pi] |= mask;
			val >>= 1;
		}
		mask <<= 1;
	}
}
