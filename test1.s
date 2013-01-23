	.equ TARGET_BAUD_RATE, 115200
	
	.equ SYSTEM_CLOCK, 19200000
	
	.equ GPFSEL1,   0x7e200004
	.equ GPSET0,    0x7e20001C
	.equ GPCLR0,    0x7e200028
 	.equ GPPUD,     0x7e200094
	.equ GPPUDCLK0, 0x7e200098
	
	.equ AUX_ENABLES,     0x7e215004
	.equ AUX_MU_IO_REG,   0x7e215040
	.equ AUX_MU_IER_REG,  0x7e215044
	.equ AUX_MU_IIR_REG,  0x7e215048
	.equ AUX_MU_LCR_REG,  0x7e21504C
	.equ AUX_MU_MCR_REG,  0x7e215050
	.equ AUX_MU_LSR_REG,  0x7e215054
	.equ AUX_MU_MSR_REG,  0x7e215058
	.equ AUX_MU_SCRATCH,  0x7e21505C
	.equ AUX_MU_CNTL_REG, 0x7e215060
	.equ AUX_MU_STAT_REG, 0x7e215064
	.equ AUX_MU_BAUD_REG, 0x7e215068

	.equ	BIG_NUM, 0x12345678
	.global BIG_NUM
.text
	.global _start
_start:	
	b	__start

	; Some test opcodes
	nop
	bl	r3
;	bl	(r3)
;	bl	--(r3)
;	bl	(r3)++
	b	r9
;	bcc	r10
	push r6
	push r5
	push lr
	pop r7
	push r6, lr
;	push r6, pc
	pop r6, pc
	push r6-r8
	push	r6-r8,lr
	pop	r6
	
	.fill 0x200, 0

	.macro poke reg, val
	mov	r1, \reg
	mov	r0, \val
	st	r0, (r1)
	.endm

__start:
	mov 	r1, GPFSEL1
	ld 	r0, (r1)
	and 	r0, ~(7<<12)
	or 	r0, 2<<12
	st 	r0, (r1)

	poke	GPPUD, 0

	mov 	r0, 0
delay1:
	nop
	add 	r0, 1
	cmp 	r0, 150
	bne 	delay1
	
	poke	GPPUDCLK0, 1 << 14

	mov 	r0, 0
delay2:	
	nop
	add 	r0, 1
	cmp 	r0, 150
	bne 	delay2

	poke	GPPUDCLK0, 0
	
	; Set up serial port
	poke	AUX_ENABLES, 1

	.equ BAUD_REG, ((SYSTEM_CLOCK/(TARGET_BAUD_RATE*8))-1)
	poke	AUX_MU_IER_REG, 0
	poke	AUX_MU_CNTL_REG, 0
	poke	AUX_MU_LCR_REG, 3
	poke	AUX_MU_MCR_REG, 0
	poke	AUX_MU_IER_REG, 0
	poke	AUX_MU_IIR_REG, 0xC6
	poke	AUX_MU_BAUD_REG, BAUD_REG
	poke	AUX_MU_LCR_REG, 0x03
	poke	AUX_MU_CNTL_REG, 2
	
	mov	r0, 0x12345678
	bl	hexstring

	mov 	r2, 0
loop:
	or	r2, 0x30
	bl	putchar
	
	add 	r2, 1
	and 	r2, 7
	b 	loop

; 	nop
; 	nop

	.global delay3
	
	b	delay3
	nop
	nop

hexstring:
	push	r6, lr
	mov	r3, 0
	
hexstring_loop:	
	mov 	r1, r0

	lsr 	r1, 28
	and 	r1, 0xf
	
	ldb 	r2, digits(r1)
	
	lea 	r2, digits	;
	add 	r2, r1		;
	ldb 	r2, (r2)	;

	bl	putchar
	
	lsl 	r0, 4
	add 	r3, 1
	cmp 	r3, 8
	bne	hexstring_loop
	
	pop	r6, pc

digits:	
	.ascii	"0123456789abcdef"

putchar:
	; Wait for space in fifo
	mov 	r1, AUX_MU_LSR_REG
	ld 	r1, (r1)
	and 	r1, 0x20
	cmp 	r1, 0x20
	bne 	putchar
	
	; Push next character into serial fifo
	mov 	r1, AUX_MU_IO_REG
	st 	r2, (r1)

	rts

	scalar16 0
	scalar16 0x7fff
	scalar32_1 0, 0x1234
	scalar32_1 0x3fff, 0x1234
	scalar32_2 0, 0x1234
	scalar32_2 0x1fff, 0x1234
	scalar48 0, 0x12345678
	scalar48 0x0fff, 0x12345678
	vector48 0, 0x12345678
	vector48 0x07ff, 0x12345678
	vector80 0, 0x12345678, 0x9abcdef0
	vector80 0x07ff, 0x12345678, 0x9abcdef0
