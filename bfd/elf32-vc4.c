/* VC4-specific support for 32-bit ELF.
   Copyright 2012
   Free Software Foundation, Inc.
   Contributed by Johan Rydberg, jrydberg@opencores.org

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/vc4.h"
#include "libiberty.h"
#include <assert.h>
#include <ctype.h>

bfd_reloc_status_type
vc4_elf_reloc (bfd *abfd ATTRIBUTE_UNUSED,
	       arelent *reloc_entry,
	       asymbol *symbol ATTRIBUTE_UNUSED,
	       void *data ATTRIBUTE_UNUSED,
	       asection *input_section,
	       bfd *output_bfd,
	       char **error_message ATTRIBUTE_UNUSED);

static reloc_howto_type vc4_elf_howto_table[] =
{
  /* This reloc does nothing.  */
  HOWTO (R_VC4_NONE,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_NONE",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 7 bit relocation.  */
  HOWTO (R_VC4_PCREL7_MUL2,    /* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 7,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL7",       /* name */
	 FALSE,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0x007f,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 8 bit relocation.  */
  HOWTO (R_VC4_PCREL8_MUL2,      /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 TRUE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL8",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x00ff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 10 bit relocation.  */
  HOWTO (R_VC4_PCREL10_MUL2,   /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 10,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL10",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x03ff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 16 bit relocation.  */
  HOWTO (R_VC4_PCREL16,        /* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL16",      /* name */
	 FALSE,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 23 bit relocation.  */
  HOWTO (R_VC4_PCREL23_MUL2,   /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 23,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL23",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x007fffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 27 bit relocation.  */
  HOWTO (R_VC4_PCREL27,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL27",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x07ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 27 bit relocation.  */
  HOWTO (R_VC4_PCREL27_MUL2,   /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL27",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x07ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 32 bit relocation.  */
  HOWTO (R_VC4_PCREL32,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL32",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* A absolute 5 bit relocation.  */
  HOWTO (R_VC4_IMM5_1,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM5_1",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 5 bit relocation.  */
  HOWTO (R_VC4_IMM5_2,         /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM5_2",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 6 bit relocation.  */
  HOWTO (R_VC4_IMM6,           /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM6",         /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 6 bit relocation.  */
  HOWTO (R_VC4_IMM6_MUL4,      /* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM6_MUL4",     /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 11 bit relocation.  */
  HOWTO (R_VC4_IMM11,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 11,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM11",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 12 bit relocation.  */
  HOWTO (R_VC4_IMM12,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 12,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM11",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 16 bit relocation.  */
  HOWTO (R_VC4_IMM16,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM16",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 23 bit relocation.  */
  HOWTO (R_VC4_IMM23,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 23,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM23",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 27 bit relocation.  */
  HOWTO (R_VC4_IMM27,           /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM27",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 32 bit relocation.  */
  HOWTO (R_VC4_IMM32,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM32",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 32 bit relocation.  */
  HOWTO (R_VC4_IMM32_2,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM32_2",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
};

/* Map BFD reloc types to Vc4 ELF reloc types.  */

struct vc4_bfd_fixup_table
{
  char code;
  int pc_rel;
  int divide;
  const char *str;
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned int vc4_reloc_val;
};

struct vc4_bfd_fixup_table2
{
  size_t width;
  size_t length;
  uint128_t mask;
};

static const struct vc4_bfd_fixup_table bfd_fixup_table[] =
  {
    { ' ', 0, 0, "", BFD_RELOC_NONE, R_VC4_NONE },
    { 'o', 1, 2, "0001 1ccc cooo oooo", BFD_RELOC_VC4_REL7_MUL2, R_VC4_PCREL7_MUL2 }, /* b%s<c> 0x%08x<$+o*2> */
    { 'o', 1, 2, "1000 cccc aaaa dddd 10uu uuuu oooo oooo", BFD_RELOC_VC4_REL8_MUL2, R_VC4_PCREL8_MUL2 }, /* addcmpb%s<c> r%i<d>, r%i<a>, #%i<u>, 0x%08x<$+o*2> */
    { 'o', 1, 2, "1000 cccc aaaa dddd 00ss ssoo oooo oooo", BFD_RELOC_VC4_REL10_MUL2, R_VC4_PCREL10_MUL2 }, /*  */
    { 'o', 1, 1, "1011 1111 111d dddd oooo oooo oooo oooo", BFD_RELOC_VC4_REL16, R_VC4_PCREL16 }, /*  */
    { 'o', 1, 2, "1001 cccc 0ooo oooo oooo oooo oooo oooo", BFD_RELOC_VC4_REL23_MUL2, R_VC4_PCREL23_MUL2}, /*  */
    { 'o', 1, 1, "1110 0111 ww0d dddd 1111 1ooo oooo oooo oooo oooo oooo oooo", BFD_RELOC_VC4_REL27, R_VC4_PCREL27 }, /*  */
    { 'o', 1, 2, "1001 oooo 1ooo oooo oooo oooo oooo oooo", BFD_RELOC_VC4_REL27_MUL2, R_VC4_PCREL27_MUL2 }, /*  */
    { 'o', 1, 1, "1110 0101 000d dddd oooo oooo oooo oooo oooo oooo oooo oooo", BFD_RELOC_VC4_REL32, R_VC4_PCREL32 }, /*  */
    { 'o', 0, 1, "0000 010o oooo dddd", BFD_RELOC_VC4_IMM5_1, R_VC4_IMM5_1 }, /*  */
    { 'u', 0, 1, "1010 0000 ww1d dddd aaaa accc c10u uuuu", BFD_RELOC_VC4_IMM5_2, R_VC4_IMM5_2 }, /*  */
    { ' ', 0, 1, "", BFD_RELOC_VC4_IMM6, R_VC4_IMM6 }, /*  */
    { 'o', 0, 4, "0001 0ooo ooo1 1001", BFD_RELOC_VC4_IMM6_MUL4, R_VC4_IMM6_MUL4 }, /*  */
    { 'x', 0, 1, "1111 0xxx xxxx xxxx yyyy yyyy yyyy yyyy yyyy yyyy yyyy yyyy", BFD_RELOC_VC4_IMM11, R_VC4_IMM11 }, /*  */
    { 'o', 0, 1, "1010 001o ww0d dddd ssss sooo oooo oooo", BFD_RELOC_VC4_IMM12, R_VC4_IMM12 }, /*  */
    { 'o', 0, 1, "1010 1000 ww0d dddd oooo oooo oooo oooo", BFD_RELOC_VC4_IMM16, R_VC4_IMM16 }, /*  */
    { ' ', 0, 0, "", BFD_RELOC_VC4_IMM23, R_VC4_IMM23 }, /*  */
    { 'o', 0, 1, "1110 0111 ww0d dddd ssss sooo oooo oooo oooo oooo oooo oooo", BFD_RELOC_VC4_IMM27, R_VC4_IMM27 }, /*  */
    { 'u', 0, 1, "1110 0000 ssss dddd uuuu uuuu uuuu uuuu uuuu uuuu uuuu uuuu", BFD_RELOC_VC4_IMM32, R_VC4_IMM32 }, /*  */
    { 'z', 0, 1, "1111 1xxx xxxx xxxx yyyy yyyy yyyy yyyy yyyy yyyy yyyy yyyy zzzz zzzz zzzz zzzz zzzz zzzz zzzz zzzz", BFD_RELOC_VC4_IMM32_2, R_VC4_IMM32_2 }, /*  */
  };
#define BFD_FIXUP_COUNT ARRAY_SIZE(bfd_fixup_table)

static struct vc4_bfd_fixup_table2 bfd_fixup_table2[BFD_FIXUP_COUNT];

static reloc_howto_type *
vc4_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
		       bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = ARRAY_SIZE(bfd_fixup_table); --i;)
    if (bfd_fixup_table[i].bfd_reloc_val == code) {
      unsigned int val = bfd_fixup_table[i].vc4_reloc_val;
      printf("%s %d %d %d\n", __FUNCTION__, code, val, vc4_elf_howto_table[val].type);

      assert(vc4_elf_howto_table[val].type == val);

      return &vc4_elf_howto_table[val];
    }

  return NULL;
}

static reloc_howto_type *
vc4_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
		       const char *r_name)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE(vc4_elf_howto_table); i++)
    if (vc4_elf_howto_table[i].name != NULL &&
	strcasecmp(vc4_elf_howto_table[i].name, r_name) == 0)
      return &vc4_elf_howto_table[i];

  return NULL;
}

/* Set the howto pointer for an Vc4 ELF reloc.  */

static void
vc4_info_to_howto_rela (bfd * abfd ATTRIBUTE_UNUSED,
			arelent * cache_ptr,
			Elf_Internal_Rela * dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_VC4_max);
  cache_ptr->howto = & vc4_elf_howto_table[r_type];
}

static void make_tab2(void);

static void vc4_poke_reloc_value(bfd *input_bfd, bfd_byte *contents, unsigned int type, bfd_signed_vma srel)
{
  size_t i, len;
  uint16_t ins[5];

  make_tab2();

  switch (type)
    {
    case R_VC4_PCREL7_MUL2:
    case R_VC4_PCREL8_MUL2:
    case R_VC4_PCREL10_MUL2:
    case R_VC4_PCREL16:
    case R_VC4_PCREL23_MUL2:
    case R_VC4_PCREL27:
    case R_VC4_PCREL27_MUL2:
    case R_VC4_PCREL32:
    case R_VC4_IMM5_1:
    case R_VC4_IMM5_2:
    case R_VC4_IMM6:
    case R_VC4_IMM6_MUL4:
    case R_VC4_IMM11:
    case R_VC4_IMM12:
    case R_VC4_IMM16:
    case R_VC4_IMM23:
    case R_VC4_IMM27:
    case R_VC4_IMM32:
    case R_VC4_IMM32_2:
      len = vc4_bfd_fixup_get_length(type);
	  
      for (i = 0; i < len; i++) {
	ins[i] = bfd_get_16(input_bfd, contents + i * 2);
      }

      vc4_bfd_fixup_set(bfd_fixup_table[type].bfd_reloc_val, ins, srel);

      for (i = 0; i < len; i++) {
	bfd_put_16(input_bfd, ins[i], contents + i * 2);
      }
      break;

    default:
      printf("bad %s!\n", __FUNCTION__);
      abort();
      break;
    }
}

/* Perform a single relocation.  By default we use the standard BFD
   routines, but a few relocs, we have to do them ourselves.  */

static bfd_reloc_status_type
vc4_final_link_relocate (reloc_howto_type *howto,
			 bfd *input_bfd,
			 asection *input_section,
			 bfd_byte *contents,
			 Elf_Internal_Rela *rel,
			 bfd_vma relocation)
{
  bfd_reloc_status_type r = bfd_reloc_ok;
  bfd_signed_vma srel;

  switch (howto->type)
    {
    case R_VC4_PCREL7_MUL2:
    case R_VC4_PCREL8_MUL2:
    case R_VC4_PCREL10_MUL2:
    case R_VC4_PCREL16:
    case R_VC4_PCREL23_MUL2:
    case R_VC4_PCREL27:
    case R_VC4_PCREL27_MUL2:
    case R_VC4_PCREL32:
    case R_VC4_IMM5_1:
    case R_VC4_IMM5_2:
    case R_VC4_IMM6:
    case R_VC4_IMM6_MUL4:
    case R_VC4_IMM11:
    case R_VC4_IMM12:
    case R_VC4_IMM16:
    case R_VC4_IMM23:
    case R_VC4_IMM27:
    case R_VC4_IMM32:
    case R_VC4_IMM32_2:

      contents += rel->r_offset;
      srel = (bfd_signed_vma) relocation;
      srel += rel->r_addend;
      if (howto->pc_relative)
	{
	  srel -= (input_section->output_section->vma +
		   input_section->output_offset);
	  if (howto->pcrel_offset)
	    relocation -= rel->r_offset;
	}

      if (howto->rightshift != 0)
	{
	  if (srel & 1)
	    return bfd_reloc_outofrange;
	  srel >>= 1;
	  if (howto->rightshift > 1)
	    {
	      if (srel & 1)
		return bfd_reloc_outofrange;
	      srel >>= 1;
	    }
	}

      vc4_poke_reloc_value(input_bfd, contents, howto->type, srel);

      break;

    default:
      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);
      break;
    }

  return r;
}

/* Relocate an Vc4 ELF section.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjusting the section contents as
   necessary, and (if using Rela relocs and generating a relocatable
   output file) adjusting the reloc addend as necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocatable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static bfd_boolean
vc4_elf_relocate_section (bfd *output_bfd,
			       struct bfd_link_info *info,
			       bfd *input_bfd,
			       asection *input_section,
			       bfd_byte *contents,
			       Elf_Internal_Rela *relocs,
			       Elf_Internal_Sym *local_syms,
			       asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  printf("vc4_elf_relocate_section\n");

  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *name = NULL;
      int r_type;

      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);

      if (r_type == R_VC4_GNU_VTINHERIT
	  || r_type == R_VC4_GNU_VTENTRY)
	continue;

      assert((unsigned int) r_type <
	     (sizeof vc4_elf_howto_table / sizeof (reloc_howto_type)));

      if ((unsigned int) r_type >=
	  (sizeof vc4_elf_howto_table / sizeof (reloc_howto_type)))
	abort ();

      howto = vc4_elf_howto_table + ELF32_R_TYPE (rel->r_info);
      h = NULL;
      sym = NULL;
      sec = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  bfd_boolean unresolved_reloc, warned;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned);
	}

      if (sec != NULL && discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, 1, relend, howto, 0, contents);

      if (info->relocatable)
	continue;

      r = vc4_final_link_relocate (howto, input_bfd, input_section,
					contents, rel, relocation);

      if (r != bfd_reloc_ok)
	{
	  const char *msg = NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      r = info->callbacks->reloc_overflow
		(info, (h ? &h->root : NULL), name, howto->name,
		 (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      r = info->callbacks->undefined_symbol
		(info, name, input_bfd, input_section, rel->r_offset, TRUE);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    r = info->callbacks->warning
	      (info, msg, name, input_bfd, input_section, rel->r_offset);

	  if (!r)
	    return FALSE;
	}
    }

  return TRUE;
}

bfd_reloc_status_type
vc4_elf_reloc (bfd *abfd ATTRIBUTE_UNUSED,
				arelent *reloc_entry,
				asymbol *symbol ATTRIBUTE_UNUSED,
				void *data ATTRIBUTE_UNUSED,
				asection *input_section,
				bfd *output_bfd,
				char **error_message ATTRIBUTE_UNUSED)
{
  bfd_vma relocation;
  bfd_reloc_status_type flag = bfd_reloc_ok;
  bfd_size_type octets = reloc_entry->address * bfd_octets_per_byte (abfd);
  bfd_vma output_base = 0;
  reloc_howto_type *howto = reloc_entry->howto;
  asection *reloc_target_output_section;

  printf("%s\n", __FUNCTION__);

  if (bfd_is_abs_section (symbol->section)
      && output_bfd != NULL)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }
  printf("%s a\n", __FUNCTION__);

  /* If we are not producing relocatable output, return an error if
     the symbol is not defined.  An undefined weak symbol is
     considered to have a value of zero (SVR4 ABI, p. 4-27).  */
  if (bfd_is_und_section (symbol->section)
      && (symbol->flags & BSF_WEAK) == 0
      && output_bfd == NULL)
    flag = bfd_reloc_undefined;

  /* Is the address of the relocation really within the section?  */
  if (reloc_entry->address > bfd_get_section_limit (abfd, input_section))
    return bfd_reloc_outofrange;
  printf("%s c\n", __FUNCTION__);

  /* Work out which section the relocation is targeted at and the
     initial relocation command value.  */

  /* Get symbol value.  (Common symbols are special.)  */
  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  reloc_target_output_section = symbol->section->output_section;

  /* Convert input-section-relative symbol value to absolute.  */
  if ((output_bfd && ! howto->partial_inplace)
      || reloc_target_output_section == NULL)
    output_base = 0;
  else
    output_base = reloc_target_output_section->vma;

  relocation += output_base + symbol->section->output_offset;

  /* Add in supplied addend.  */
  relocation += reloc_entry->addend;

  /* Here the variable relocation holds the final address of the
     symbol we are relocating against, plus any addend.  */

  if (howto->pc_relative)
    {
      /* This is a PC relative relocation.  We want to set RELOCATION
	 to the distance between the address of the symbol and the
	 location.  RELOCATION is already the address of the symbol.

	 We start by subtracting the address of the section containing
	 the location.

	 If pcrel_offset is set, we must further subtract the position
	 of the location within the section.  Some targets arrange for
	 the addend to be the negative of the position of the location
	 within the section; for example, i386-aout does this.  For
	 i386-aout, pcrel_offset is FALSE.  Some other targets do not
	 include the position of the location; for example, m88kbcs,
	 or ELF.  For those targets, pcrel_offset is TRUE.

	 If we are producing relocatable output, then we must ensure
	 that this reloc will be correctly computed when the final
	 relocation is done.  If pcrel_offset is FALSE we want to wind
	 up with the negative of the location within the section,
	 which means we must adjust the existing addend by the change
	 in the location within the section.  If pcrel_offset is TRUE
	 we do not want to adjust the existing addend at all.

	 FIXME: This seems logical to me, but for the case of
	 producing relocatable output it is not what the code
	 actually does.  I don't want to change it, because it seems
	 far too likely that something will break.  */

      relocation -=
	input_section->output_section->vma + input_section->output_offset;

      if (howto->pcrel_offset)
	relocation -= reloc_entry->address;
    }
  printf("%s d\n", __FUNCTION__);

  if (output_bfd != NULL)
    {
      if (! howto->partial_inplace)
	{
	  /* This is a partial relocation, and we want to apply the relocation
	     to the reloc entry rather than the raw data. Modify the reloc
	     inplace to reflect what we now know.  */
	  reloc_entry->addend = relocation;
	  reloc_entry->address += input_section->output_offset;
	  return flag;
	}
      else
	{
	  /* This is a partial relocation, but inplace, so modify the
	     reloc record a bit.

	     If we've relocated with a symbol with a section, change
	     into a ref to the section belonging to the symbol.  */

	  reloc_entry->address += input_section->output_offset;

	  reloc_entry->addend = relocation;
	}
    }
  else
    {
      reloc_entry->addend = 0;
    }

  /* FIXME: This overflow checking is incomplete, because the value
     might have overflowed before we get here.  For a correct check we
     need to compute the value in a size larger than bitsize, but we
     can't reasonably do that for a reloc the same size as a host
     machine word.
     FIXME: We should also do overflow checking on the result after
     adding in the value contained in the object file.  */

  if (howto->complain_on_overflow != complain_overflow_dont
      && flag == bfd_reloc_ok)
    flag = bfd_check_overflow (howto->complain_on_overflow,
			       howto->bitsize,
			       howto->rightshift,
			       bfd_arch_bits_per_address (abfd),
			       relocation);

  printf("%s %d\n", __FUNCTION__, (int)relocation);

  vc4_poke_reloc_value(abfd, (unsigned char *) data + octets, howto->type, relocation);

  return flag;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
vc4_elf_gc_mark_hook (asection *sec,
			   struct bfd_link_info *info,
			   Elf_Internal_Rela *rel,
			   struct elf_link_hash_entry *h,
			   Elf_Internal_Sym *sym)
{
  if (h != NULL)
    switch (ELF32_R_TYPE (rel->r_info))
      {
      case R_VC4_GNU_VTINHERIT:
      case R_VC4_GNU_VTENTRY:
	return NULL;
      }

  return _bfd_elf_gc_mark_hook (sec, info, rel, h, sym);
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */

static bfd_boolean
vc4_elf_check_relocs (bfd *abfd,
			   struct bfd_link_info *info,
			   asection *sec,
			   const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;

  if (info->relocatable)
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	}

      switch (ELF32_R_TYPE (rel->r_info))
	{
	  /* This relocation describes the C++ object vtable hierarchy.
	     Reconstruct it for later use during GC.  */
	case R_VC4_GNU_VTINHERIT:
	  if (!bfd_elf_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return FALSE;
	  break;

	  /* This relocation describes which C++ vtable entries are actually
	     used.  Record for later use during GC.  */
	case R_VC4_GNU_VTENTRY:
	  BFD_ASSERT (h != NULL);
	  if (h != NULL
	      && !bfd_elf_gc_record_vtentry (abfd, sec, h, rel->r_addend))
	    return FALSE;
	  break;
	}
    }

  return TRUE;
}

/* Set the right machine number.  */

static bfd_boolean
vc4_elf_object_p (bfd *abfd)
{
  bfd_default_set_arch_mach (abfd, bfd_arch_vc4, bfd_mach_vc4);
  return TRUE;
}

/* Store the machine number in the flags field.  */

static void
vc4_elf_final_write_processing (bfd *abfd,
				bfd_boolean linker ATTRIBUTE_UNUSED)
{
  unsigned long val;

  switch (bfd_get_mach (abfd))
    {
    case bfd_mach_vc4:
    default:
      val = 0;
      break;
      /*abort ();*/
    }

  elf_elfheader (abfd)->e_flags &= ~0xf;
  elf_elfheader (abfd)->e_flags |= val;
}


void uint128_shl(uint128_t *v, size_t count)
{
  while (count-- != 0) {
    v->hi <<= 1;
    if ((v->lo & (1uLL << 63)) != 0) {
      v->hi |= 1uLL;
    }
    v->lo <<= 1;
  }
}

void uint128_shr(uint128_t *v, size_t count)
{
  while (count-- != 0) {
    v->lo >>= 1;
    if ((v->hi & 1u) != 0) {
      v->lo |= (1uLL << 63);
    }
    v->hi >>= 1;
  }
}

static int bfd_fixup_table_done;

static void string_to_mask(const char *str, char code, uint128_t *maskp, size_t *lengthp)
{
  uint128_t mask;
  size_t length, count;
  uint16_t wm, wc, i;
  uint128_t mask2;

  mask.hi = mask.lo = 0;
  mask2.hi = mask2.lo = 0;
  length = 0;
  count = 0;
  wm = 0x8000;
  wc = 0;
  i = 0;
  for (; *str; str++ ) {
    if (isblank(*str)) {
      continue;
    }

    if (*str == code) {
      wc |= wm;
      length++;
    }
    count++;

    wm >>= 1;
    if (wm == 0) {
      mask2.hi = 0;
      mask2.lo = wc;
      uint128_shl(&mask2, i * 16);
      mask.lo |= mask2.lo;
      mask.hi |= mask2.hi;
      wm = 0x8000;
      wc = 0;
      i++;
    }
  }

  assert(wm == 0x8000);
  assert((count % 16) == 0);

  if (lengthp != NULL) 
    *lengthp = length;

  if (maskp != NULL)
    *maskp = mask;
}

static void make_tab2(void)
{
  size_t i;
  uint128_t mask;
  size_t len;

  if (bfd_fixup_table_done)
    return;
  bfd_fixup_table_done = 1;

  for (i = 0; i < BFD_FIXUP_COUNT; i++) {

    assert(i == bfd_fixup_table[i].vc4_reloc_val);
    assert(i == vc4_elf_howto_table[i].type);

    string_to_mask(bfd_fixup_table[i].str,
		   bfd_fixup_table[i].code,
		   &bfd_fixup_table2[i].mask,
		   &bfd_fixup_table2[i].width);

    if (strlen(bfd_fixup_table[i].str) == 0)
      continue;
      
    mask = bfd_fixup_table2[i].mask;
    len = 0;
    while (mask.hi != 0 || mask.lo != 0) {
      len++;
      uint128_shr(&mask, 16);
    }
    if (len == 0) {
      fprintf(stderr, "Bad len %d %d %llx:%llx\n",
	      bfd_fixup_table[i].bfd_reloc_val, i,
	      bfd_fixup_table2[i].mask.hi,
	      bfd_fixup_table2[i].mask.lo);
    }
    assert(len > 0 && len <= 5);
    bfd_fixup_table2[i].length = len;
  }
/*
  for (i = 0; i < BFD_FIXUP_COUNT; i++) {
    printf("TAB %4d %2d %d %016llx%016llx %-20s %s\n",
	   bfd_fixup_table[i].bfd_reloc_val, i,
	   bfd_fixup_table2[i].length,
	   bfd_fixup_table2[i].mask.hi,
	   bfd_fixup_table2[i].mask.lo,
	   vc4_elf_howto_table[i].name,
	   bfd_fixup_table[i].str);
  }
*/
}

bfd_reloc_code_real_type vc4_bfd_fixup_get(const char *str, char code, int pc_rel, int divide)
{
  size_t i;
  uint128_t mask;
  size_t width;

  string_to_mask(str, code, &mask, &width);

  make_tab2();

  for (i = 0; i < BFD_FIXUP_COUNT; i++) {
    if (mask.hi == bfd_fixup_table2[i].mask.hi &&
	mask.lo == bfd_fixup_table2[i].mask.lo &&
	width == bfd_fixup_table2[i].width &&
	pc_rel == bfd_fixup_table[i].pc_rel &&
	divide == bfd_fixup_table[i].divide) {
      return bfd_fixup_table[i].bfd_reloc_val;
    }
  }

  return 0;
}

size_t vc4_bfd_fixup_get_elf(bfd_reloc_code_real_type bfd_fixup)
{
  size_t i;

  make_tab2();

  for (i = 0; i < BFD_FIXUP_COUNT; i++) {
    if (bfd_fixup_table[i].bfd_reloc_val == bfd_fixup) {
      return i;
    }
  }

  return R_VC4_NONE;
}

size_t vc4_bfd_fixup_get_width(bfd_reloc_code_real_type bfd_fixup)
{
  size_t i;

  make_tab2();

  for (i = 0; i < BFD_FIXUP_COUNT; i++) {
    if (bfd_fixup_table[i].bfd_reloc_val == bfd_fixup) {
      return bfd_fixup_table2[i].width;
    }
  }

  return 0;
}

size_t vc4_bfd_fixup_get_divide(bfd_reloc_code_real_type bfd_fixup)
{
  size_t i;

  for (i = 0; i < BFD_FIXUP_COUNT; i++) {
    if (bfd_fixup_table[i].bfd_reloc_val == bfd_fixup) {
      return bfd_fixup_table[i].divide;
    }
  }

  return 0;
}

size_t vc4_bfd_fixup_get_length(bfd_reloc_code_real_type bfd_fixup)
{
  size_t i;

  make_tab2();

  for (i = 0; i < BFD_FIXUP_COUNT; i++) {
    if (bfd_fixup_table[i].bfd_reloc_val == bfd_fixup) {
      return bfd_fixup_table2[i].length;
    }
  }

  return 0;
}

uint128_t vc4_bfd_fixup_get_mask(bfd_reloc_code_real_type bfd_fixup)
{
  size_t i;
  uint128_t mask = {0,0};

  make_tab2();

  for (i = 0; i < BFD_FIXUP_COUNT; i++) {
    if (bfd_fixup_table[i].bfd_reloc_val == bfd_fixup) {
      return bfd_fixup_table2[i].mask;
    }
  }

  return mask;
}

static char *dump_uint16s(char *buf, const uint16_t *dat, size_t len)
{
  size_t i, o;

  for (i = 0, o = 0; i < len; i++) {
    o += sprintf(buf + o, " %04x", dat[i]);
  }
  return buf + 1;
}

void vc4_bfd_fixup_set(bfd_reloc_code_real_type bfd_fixup, uint16_t *ins, long val)
{
  uint128_t mask = vc4_bfd_fixup_get_mask(bfd_fixup);
  size_t len = vc4_bfd_fixup_get_length(bfd_fixup);
  size_t word_pos = 0;
  uint16_t bit_pos = 1;
  char buf[80];

  printf("%s: %s = %ld (%d)  %llx%llx\n", __func__,
	 dump_uint16s(buf, ins, len),
	 val, bfd_fixup, mask.hi, mask.lo);

  while (mask.hi != 0 || mask.lo != 0) {

    if (mask.lo & 1) {
      if (val & 1) {
	ins[word_pos] |= bit_pos;
      } else {
	ins[word_pos] &= ~bit_pos;
      }
      val >>= 1;
    }

    uint128_shr(&mask, 1);

    bit_pos <<= 1;
    if (bit_pos == 0) {
      bit_pos = 0x0001;
      word_pos++;
    }
  }

  printf("%s: %s done\n", __func__, dump_uint16s(buf, ins, len));
}


#define ELF_ARCH			bfd_arch_vc4
#define ELF_MACHINE_CODE		EM_VIDEOCORE3
#define ELF_MACHINE_ALT1		EM_VIDEOCORE
#define ELF_MAXPAGESIZE			0x1000

#define TARGET_LITTLE_SYM		bfd_elf32_vc4_vec
#define TARGET_LITTLE_NAME		"elf32-vc4"

#define elf_info_to_howto_rel		NULL
#define elf_info_to_howto		vc4_info_to_howto_rela
#define elf_backend_relocate_section	vc4_elf_relocate_section
#define elf_backend_gc_mark_hook	vc4_elf_gc_mark_hook
#define elf_backend_check_relocs	vc4_elf_check_relocs

#define elf_backend_can_gc_sections	1
#define elf_backend_rela_normal		1

#define bfd_elf32_bfd_reloc_type_lookup vc4_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup vc4_reloc_name_lookup

#define elf_backend_object_p                vc4_elf_object_p
#define elf_backend_final_write_processing  vc4_elf_final_write_processing

#include "elf32-target.h"
