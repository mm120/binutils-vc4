#objdump: -d
#name: Pushing and Poping

.*: .*

Disassembly of section .text:

00000000 <.text>:
   0:	02a0                     	push r6
   2:	0220                     	pop  r6
   4:	03a0                     	push r6, lr
   6:	0320                     	pop  r6, pc
   8:	0280                     	push r0
   a:	02c0                     	push r16
   c:	02e0                     	push r24
   e:	0200                     	pop  r0
  10:	0240                     	pop  r16
  12:	0260                     	pop  r24
  14:	0380                     	push r0, lr
  16:	03c0                     	push r16, lr
  18:	03e0                     	push r24, lr
  1a:	0300                     	pop  r0, pc
  1c:	0340                     	pop  r16, pc
  1e:	0360                     	pop  r24, pc
  20:	02bf                     	push r6-r5
  22:	02a0                     	push r6
  24:	02a1                     	push r6-r7
  26:	02ba                     	push r6-r0
  28:	02b9                     	push r6-r31
  2a:	03bf                     	push r6-r5, lr
  2c:	03a0                     	push r6, lr
  2e:	03a1                     	push r6-r7, lr
  30:	03ba                     	push r6-r0, lr
  32:	03b9                     	push r6-r31, lr
  34:	023f                     	pop  r6-r5
  36:	0220                     	pop  r6
  38:	0221                     	pop  r6-r7
  3a:	023a                     	pop  r6-r0
  3c:	0239                     	pop  r6-r31
  3e:	033f                     	pop  r6-r5, pc
  40:	0320                     	pop  r6, pc
  42:	0321                     	pop  r6-r7, pc
  44:	033a                     	pop  r6-r0, pc
  46:	0339                     	pop  r6-r31, pc
  48:	0285                     	push r0-r5
  4a:	0286                     	push r0-r6
  4c:	0287                     	push r0-r7
  4e:	0280                     	push r0
  50:	029f                     	push r0-r31
  52:	0385                     	push r0-r5, lr
  54:	0386                     	push r0-r6, lr
  56:	0387                     	push r0-r7, lr
  58:	0380                     	push r0, lr
  5a:	039f                     	push r0-r31, lr
  5c:	0205                     	pop  r0-r5
  5e:	0206                     	pop  r0-r6
  60:	0207                     	pop  r0-r7
  62:	0200                     	pop  r0
  64:	021f                     	pop  r0-r31
  66:	0305                     	pop  r0-r5, pc
  68:	0306                     	pop  r0-r6, pc
  6a:	0307                     	pop  r0-r7, pc
  6c:	0300                     	pop  r0, pc
  6e:	031f                     	pop  r0-r31, pc
  70:	02d5                     	push r16-r5
  72:	02d6                     	push r16-r6
  74:	02d7                     	push r16-r7
  76:	02d0                     	push r16-r0
  78:	02cf                     	push r16-r31
  7a:	03d5                     	push r16-r5, lr
  7c:	03d6                     	push r16-r6, lr
  7e:	03d7                     	push r16-r7, lr
  80:	03d0                     	push r16-r0, lr
  82:	03cf                     	push r16-r31, lr
  84:	0255                     	pop  r16-r5
  86:	0256                     	pop  r16-r6
  88:	0257                     	pop  r16-r7
  8a:	0250                     	pop  r16-r0
  8c:	024f                     	pop  r16-r31
  8e:	0355                     	pop  r16-r5, pc
  90:	0356                     	pop  r16-r6, pc
  92:	0357                     	pop  r16-r7, pc
  94:	0350                     	pop  r16-r0, pc
  96:	034f                     	pop  r16-r31, pc
  98:	02ed                     	push r24-r5
  9a:	02ee                     	push r24-r6
  9c:	02ef                     	push r24-r7
  9e:	02e8                     	push r24-r0
  a0:	02e7                     	push r24-r31
  a2:	03ed                     	push r24-r5, lr
  a4:	03ee                     	push r24-r6, lr
  a6:	03ef                     	push r24-r7, lr
  a8:	03e8                     	push r24-r0, lr
  aa:	03e7                     	push r24-r31, lr
  ac:	026d                     	pop  r24-r5
  ae:	026e                     	pop  r24-r6
  b0:	026f                     	pop  r24-r7
  b2:	0268                     	pop  r24-r0
  b4:	0267                     	pop  r24-r31
  b6:	036d                     	pop  r24-r5, pc
  b8:	036e                     	pop  r24-r6, pc
  ba:	036f                     	pop  r24-r7, pc
  bc:	0368                     	pop  r24-r0, pc
  be:	0367                     	pop  r24-r31, pc
  c0:	a421 cf00                	push r1
  c4:	a423 cf00                	push r3
  c8:	a502 cf00                	pop r2
  cc:	a507 cf00                	pop r7
#pass
