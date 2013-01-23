#objdump: -d
#name: Simple bne (2 word instruction)

.*: .*

Disassembly of section .text:

00000000 <lab-0xe>:
   0:	0001                     	nop
   2:	0001                     	nop
   4:	0001                     	nop
   6:	0001                     	nop
   8:	0001                     	nop
   a:	0001                     	nop
   c:	0001                     	nop

0000000e <lab>:
   e:	0001                     	nop
  10:	0001                     	nop
  12:	0001                     	nop
  14:	0001                     	nop
  16:	0001                     	nop
  18:	0001                     	nop
  1a:	0001                     	nop
  1c:	0001                     	nop
  1e:	0001                     	nop
  20:	0001                     	nop
  22:	0001                     	nop
  24:	0001                     	nop
  26:	0001                     	nop
  28:	0001                     	nop
  2a:	0001                     	nop
  2c:	0001                     	nop
  2e:	0001                     	nop
  30:	0001                     	nop
  32:	0001                     	nop
  34:	0001                     	nop
  36:	0001                     	nop
  38:	0001                     	nop
  3a:	0001                     	nop
  3c:	0001                     	nop
  3e:	0001                     	nop
  40:	0001                     	nop
  42:	0001                     	nop
  44:	0001                     	nop
  46:	0001                     	nop
  48:	0001                     	nop
  4a:	0001                     	nop
  4c:	0001                     	nop
  4e:	0001                     	nop
  50:	0001                     	nop
  52:	0001                     	nop
  54:	0001                     	nop
  56:	0001                     	nop
  58:	0001                     	nop
  5a:	0001                     	nop
  5c:	0001                     	nop
  5e:	0001                     	nop
  60:	0001                     	nop
  62:	0001                     	nop
  64:	0001                     	nop
  66:	0001                     	nop
  68:	0001                     	nop
  6a:	0001                     	nop
  6c:	0001                     	nop
  6e:	0001                     	nop
  70:	0001                     	nop
  72:	0001                     	nop
  74:	0001                     	nop
  76:	0001                     	nop
  78:	0001                     	nop
  7a:	0001                     	nop
  7c:	0001                     	nop
  7e:	0001                     	nop
  80:	0001                     	nop
  82:	0001                     	nop
  84:	0001                     	nop
  86:	0001                     	nop
  88:	0001                     	nop
  8a:	0001                     	nop
  8c:	0001                     	nop
  8e:	0001                     	nop
  90:	0001                     	nop
  92:	0001                     	nop
  94:	0001                     	nop
  96:	0001                     	nop
  98:	0001                     	nop
  9a:	917f ffba                	bne 0x0000000e
#pass
