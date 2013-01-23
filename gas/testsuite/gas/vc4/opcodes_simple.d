#objdump: -d
#name: Simple values

.*: .*

Disassembly of section .text:

00000000 <.text>:
   0:	0000                     	halt
   2:	0001                     	nop
   4:	0002                     	wait
   6:	0003                     	user
   8:	0004                     	enable
   a:	0005                     	disable
   c:	0006                     	clr
   e:	0007                     	inc
  10:	0008                     	chg
  12:	0009                     	dec
  14:	000a                     	rti
  16:	0020                     	swi r0
  18:	002d                     	swi r13
  1a:	003f                     	swi r31
  1c:	005a                     	rts
  1e:	0040                     	b r0
  20:	0053                     	b r19
  22:	005f                     	b r31
  24:	0060                     	bl r0
  26:	0072                     	bl r18
  28:	007f                     	bl r31
  2a:	0082                     	tbb r2
  2c:	00a3                     	tbh r3
  2e:	00e5                     	mov r5, cpuid
#pass
