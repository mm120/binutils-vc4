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
  HOWTO (R_VC4_PCREL_7_MUL2,    /* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 7,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_7",       /* name */
	 FALSE,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0x007f,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 8 bit relocation.  */
  HOWTO (R_VC4_PCREL_8_MUL2,      /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 TRUE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_8",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x00ff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 10 bit relocation.  */
  HOWTO (R_VC4_PCREL_10_MUL2,   /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 10,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_10",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x03ff0000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 16 bit relocation.  */
  HOWTO (R_VC4_PCREL_16,        /* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 16,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_16",      /* name */
	 FALSE,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 23 bit relocation.  */
  HOWTO (R_VC4_PCREL_23_MUL2,   /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 23,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_23",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x007fffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 27 bit relocation.  */
  HOWTO (R_VC4_PCREL_27,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_27",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x07ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 27 bit relocation.  */
  HOWTO (R_VC4_PCREL_27_MUL2,   /* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_27",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x07ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A PC relative 32 bit relocation.  */
  HOWTO (R_VC4_PCREL_32,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_PCREL_32",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* A absolute 5 bit relocation.  */
  HOWTO (R_VC4_IMM_5,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_5",         /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 6 bit relocation.  */
  HOWTO (R_VC4_IMM_6,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_6",         /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 11 bit relocation.  */
  HOWTO (R_VC4_IMM_11,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 11,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_11",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 16 bit relocation.  */
  HOWTO (R_VC4_IMM_16,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_16",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 23 bit relocation.  */
  HOWTO (R_VC4_IMM_23,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 23,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_23",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 32 bit relocation.  */
  HOWTO (R_VC4_IMM_27,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 27,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_27",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 32 bit relocation.  */
  HOWTO (R_VC4_IMM_32,          /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_32",        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 32 bit relocation.  */
  HOWTO (R_VC4_IMM_32_2,        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 vc4_elf_reloc,		/* special_function */
	 "R_VC4_IMM_32_2",      /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
#if 0
  /* A PC relative 26 bit relocation, right shifted by 2.  */
  HOWTO (R_VC4_INSN_REL_26, /* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_INSN_REL_26", /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x03ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A absolute 26 bit relocation, right shifted by 2.  */
  HOWTO (R_VC4_INSN_ABS_26, /* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_INSN_ABS_26", /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x03ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_VC4_LO_16_IN_INSN, /* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_LO_16_IN_INSN", /* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_VC4_HI_16_IN_INSN, /* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_HI_16_IN_INSN",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* An 8 bit absolute relocation.  */
  HOWTO (R_VC4_8,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_8",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000,		/* src_mask */
	 0x00ff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 16 bit absolute relocation.  */
  HOWTO (R_VC4_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 32 bit absolute relocation.  */
  HOWTO (R_VC4_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_VC4_32",	/* name */
	 TRUE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* GNU extension to record C++ vtable hierarchy.  */
  HOWTO (R_VC4_GNU_VTINHERIT, /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_VC4_GNU_VTINHERIT", /* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* GNU extension to record C++ vtable member usage.  */
  HOWTO (R_VC4_GNU_VTENTRY, /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn, /* special_function */
	 "R_VC4_GNU_VTENTRY", /* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */
#endif
};

/* Map BFD reloc types to Vc4 ELF reloc types.  */

struct vc4_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned int vc4_reloc_val;
};

static const struct vc4_reloc_map vc4_reloc_map[] =
{
  { BFD_RELOC_NONE, 		R_VC4_NONE },
  { BFD_RELOC_VC4_REL7_MUL2,	R_VC4_PCREL_7_MUL2 },
  { BFD_RELOC_VC4_REL8_MUL2,	R_VC4_PCREL_8_MUL2 },
  { BFD_RELOC_VC4_REL10_MUL2,	R_VC4_PCREL_10_MUL2 },
  { BFD_RELOC_VC4_REL16,	R_VC4_PCREL_16 },
  { BFD_RELOC_VC4_REL23_MUL2,	R_VC4_PCREL_23_MUL2 },
  { BFD_RELOC_VC4_REL27,	R_VC4_PCREL_27 },
  { BFD_RELOC_VC4_REL27_MUL2,	R_VC4_PCREL_27_MUL2 },
  { BFD_RELOC_VC4_REL32,	R_VC4_PCREL_32 },

  { BFD_RELOC_VC4_IMM5,		R_VC4_IMM_5 },
  { BFD_RELOC_VC4_IMM6,		R_VC4_IMM_6 },
  { BFD_RELOC_VC4_IMM11,	R_VC4_IMM_11 },
  { BFD_RELOC_VC4_IMM16,	R_VC4_IMM_16 },
  { BFD_RELOC_VC4_IMM23,	R_VC4_IMM_23 },
  { BFD_RELOC_VC4_IMM27,	R_VC4_IMM_27 },
  { BFD_RELOC_VC4_IMM32,	R_VC4_IMM_32 },
  { BFD_RELOC_VC4_IMM32_2,	R_VC4_IMM_32_2 },

#if 0
  { BFD_RELOC_32, 		R_VC4_32 },
  { BFD_RELOC_16, 		R_VC4_16 },
  { BFD_RELOC_8, 		R_VC4_8 },
  { BFD_RELOC_HI16, 		R_VC4_HI_16_IN_INSN },
  { BFD_RELOC_LO16, 		R_VC4_LO_16_IN_INSN },
#endif
  { BFD_RELOC_VTABLE_INHERIT,	R_VC4_GNU_VTINHERIT },
  { BFD_RELOC_VTABLE_ENTRY, 	R_VC4_GNU_VTENTRY }
};

static reloc_howto_type *
vc4_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
			    bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = ARRAY_SIZE (vc4_reloc_map); --i;)
    if (vc4_reloc_map[i].bfd_reloc_val == code) {
      unsigned int val = vc4_reloc_map[i].vc4_reloc_val;
      printf("%s %d %d %d\n", __FUNCTION__, code, val, vc4_elf_howto_table[val].type);

      assert(vc4_elf_howto_table[val].type == val);

      return & vc4_elf_howto_table[val];
    }

  return NULL;
}

static reloc_howto_type *
vc4_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			    const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < (sizeof (vc4_elf_howto_table)
	    / sizeof (vc4_elf_howto_table[0]));
       i++)
    if (vc4_elf_howto_table[i].name != NULL
	&& strcasecmp (vc4_elf_howto_table[i].name, r_name) == 0)
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
  bfd_vma x;
  bfd_signed_vma srel;

  switch (howto->type)
    {
    case R_VC4_PCREL_7_MUL2:
    case R_VC4_PCREL_8_MUL2:
    case R_VC4_PCREL_10_MUL2:
    case R_VC4_PCREL_16:
    case R_VC4_PCREL_23_MUL2:
    case R_VC4_PCREL_27:
    case R_VC4_PCREL_32:
    case R_VC4_IMM_16:
    case R_VC4_IMM_27:
    case R_VC4_IMM_32:
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
	}

      switch (howto->type)
	{
	case R_VC4_PCREL_7_MUL2:
	  printf("Doing 7-bit rel %s!\n", __FUNCTION__);
	  x = bfd_get_16 (input_bfd, contents);
	  x = (x & 0xff80) | (srel & 0x7f);
	  bfd_put_16 (input_bfd, x, contents);
	  break;

	case R_VC4_PCREL_8_MUL2:
	  printf("Doing 8-bit rel %s!\n", __FUNCTION__);
	  x = bfd_get_16 (input_bfd, contents + 2);
	  x = (x & 0xff00) | (srel & 0xff);
	  bfd_put_16 (input_bfd, x, contents + 2);
	  break;

	case R_VC4_PCREL_10_MUL2:
	  printf("Doing 10-bit rel %s!\n", __FUNCTION__);
	  x = bfd_get_16 (input_bfd, contents + 2);
	  x = (x & 0xfc00) | (srel & 0x3ff);
	  bfd_put_16 (input_bfd, x, contents + 2);
	  break;

	case R_VC4_PCREL_16:
	case R_VC4_IMM_16:
	  printf("Doing 16-bit %s %s!\n", howto->type == R_VC4_PCREL_16 ? "rel" : "imm", __FUNCTION__);
	  bfd_put_16 (input_bfd, (bfd_vma)(srel & 0xffff), contents + 2);
	  break;

	case R_VC4_PCREL_23_MUL2:
	  printf("Doing 23-bit rel %s!\n", __FUNCTION__);
	  x = bfd_get_16 (input_bfd, contents);
	  x = (x & 0xff80) | ((srel >> 16) & 0x7f);
	  bfd_put_16 (input_bfd, x, contents);
	  bfd_put_16 (input_bfd, (bfd_vma)(srel & 0xffff), contents + 2);
	  break;

	case R_VC4_PCREL_27:
	case R_VC4_IMM_27:
	  printf("Doing 27-bit %s %s!\n", howto->type == R_VC4_PCREL_27 ? "rel" : "imm", __FUNCTION__);
	  x = bfd_get_16 (input_bfd, contents + 2);
	  x = (x & 0xf800) | ((srel >> 16) & 0x7ff);
	  bfd_put_16 (input_bfd, x, contents + 2);
	  bfd_put_16 (input_bfd, (bfd_vma)(srel & 0xffff), contents + 4);
	  break;

	case R_VC4_PCREL_32:
	  printf("Doing 32-bit rel %s!\n", __FUNCTION__);
	  bfd_put_16 (input_bfd, (bfd_vma)(srel & 0xffff), contents + 2);
	  bfd_put_16 (input_bfd, (bfd_vma)((srel >> 16) & 0xffff), contents + 4);
	  break;

	case R_VC4_IMM_32:
	  printf("Doing 32-bit imm %s!\n", __FUNCTION__);
	  bfd_put_16 (input_bfd, (bfd_vma)(srel & 0xffff), contents + 2);
	  bfd_put_16 (input_bfd, (bfd_vma)((srel >> 16) & 0xffff), contents + 4);
	  break;

	default:
	  printf("bad %s!\n", __FUNCTION__);
	  abort();
	  break;
	}
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
  short x;

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

  switch (howto->type)
    {
    case R_VC4_PCREL_23_MUL2:
      printf("Doing 23-bit rel %s!\n", __FUNCTION__);
      x = bfd_get_16 (abfd, (bfd_byte *) data + octets);
      x = (x & 0xff80) | ((relocation >> 16) & 0x7f);
      bfd_put_16 (abfd, (bfd_vma) x, (unsigned char *) data + octets);
      bfd_put_16 (abfd, (bfd_vma) (relocation & 0xffff), (unsigned char *) data + octets + 2);
      break;

    case R_VC4_PCREL_27:
      printf("Doing 27-bit rel %s!\n", __FUNCTION__);
      x = bfd_get_16 (abfd, (bfd_byte *) data + octets + 2);
      x = (x & 0xf800) | ((relocation >> 16) & 0x7ff);
      bfd_put_16 (abfd, (bfd_vma) x, (unsigned char *) data + octets + 2);
      bfd_put_16 (abfd, (bfd_vma) (relocation & 0xffff), (unsigned char *) data + octets + 4);
      break;

    case R_VC4_PCREL_32:
      printf("Doing 32-bit rel %s!\n", __FUNCTION__);
      bfd_put_16 (abfd, (bfd_vma) (relocation & 0xffff), (unsigned char *) data + octets + 2);
      bfd_put_16 (abfd, (bfd_vma) ((relocation >> 16) & 0xffff), (unsigned char *) data + octets + 4);
      break;

    default:
      printf("Bad reloc type!! %d %s\n", howto->type, howto->name);
      abort();
      break;
    }

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
