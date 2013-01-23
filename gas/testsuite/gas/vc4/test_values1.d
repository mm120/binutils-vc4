#objdump: -d
#name: Simple values

.*: .*

Disassembly of section .text:

00000000 <.text>:
   0:	b0e0 0020                	and r0, #0x0020
   4:	6a00                     	cmp r0, #0
   6:	6a10                     	cmp r0, #1
   8:	6bf0                     	cmp r0, #31
   a:	b140 0020                	cmp r0, #0x0020
   e:	c140 077f                	cmp r0, #-1
  12:	c140 0761                	cmp r0, #-31
  16:	c140 0760                	cmp r0, #-32
  1a:	b140 7fff                	cmp r0, #0x7fff
  1e:	e940 8000 0000           	cmp r0, #0x00008000
  24:	e940 8001 0000           	cmp r0, #0x00008001
  2a:	b140 8001                	cmp r0, #0xffff8001
  2e:	b140 8000                	cmp r0, #0xffff8000
  32:	e940 7fff ffff           	cmp r0, #0xffff7fff
  38:	c140 077e                	cmp r0, #-2
  3c:	e940 4321 8765           	cmp r0, #0x87654321
#pass
