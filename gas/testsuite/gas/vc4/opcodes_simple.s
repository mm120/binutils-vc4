

	.text

	halt
	nop
	wait
	user
	enable
	disable
	clr
	inc
	chg
	dec
	rti

	swi	r0
	swi	r13
	swi	r31
	
	rts

	b	r0
	b	r19
	b	r31

	bl	r0
	bl	r18
	bl	r31

	tbb	r2
	tbh	r3

	mov	r5, cpuid

	