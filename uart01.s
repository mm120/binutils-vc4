
.include "../vctools/vcregs.inc"
	
	.equ TARGET_BAUD_RATE, 115200
	.equ SYSTEM_CLOCK, 19200000
	
	.equ BAUD_REG, (SYSTEM_CLOCK/(TARGET_BAUD_RATE*8)) - 1

	.text
	
	.fill	0x200
	
	.macro poke reg, val
	mov	r1, \reg
	mov	r0, \val
	st	r0, (r1)
	.endm

__start:
	mov 	r1, VC_GPIO_FSEL1
	ld 	r0, (r1)
	and 	r0, ~VC_GPIO_FSEL1_FSEL14__MASK
	or 	r0, VC_GPIO_FSEL1_FSEL14_TXD0
	st 	r0, (r1)

	poke	VC_GPIO_PUD, 0

	mov 	r0, 0
delay1:
	nop
	add 	r0, 1
	cmp 	r0, 150
	bne 	delay1
	
	poke	VC_GPIO_PUDCLK0, 1 << 14

	mov 	r0, 0
delay2:	
	nop
	add 	r0, 1
	cmp 	r0, 150
	bne 	delay2

	poke	VC_GPIO_PUDCLK0, 0
	
	;; Set up serial port
	poke	VC_AUX_ENABLES, 1

	poke	VC_AUX_MU_IER_REG, 0
	poke	VC_AUX_MU_CNTL_REG, 0
	poke	VC_AUX_MU_LCR_REG, 3
	poke	VC_AUX_MU_MCR_REG, 0
	poke	VC_AUX_MU_IER_REG, 0
	poke	VC_AUX_MU_IIR_REG, 0xC6
	poke	VC_AUX_MU_BAUD_REG, BAUD_REG
	poke	VC_AUX_MU_LCR_REG, 0x03
	poke	VC_AUX_MU_CNTL_REG, 2
	
	mov 	r2, 0
loop1:
	mov 	r1, VC_AUX_MU_LSR_REG
	ld 	r1, (r1)
	and 	r1, VC_AUX_MU_LSR_REG_TX_EMPTY
	cmp 	r1, VC_AUX_MU_LSR_REG_TX_EMPTY
	bne 	loop1
	
	or	r2, '0'

	mov 	r1, VC_AUX_MU_IO_REG
	st 	r2, (r1)
	
	add 	r2, 1
	and 	r2, 7
	b 	loop1
