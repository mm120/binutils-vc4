.text
	nop
	
	.global delay3
	.extern delay3
delay3:
	nop
	nop
	b	delay3

	mov	r9, cpuid
	rti
	halt
	nop
	b r10
	bl r11
	tbb r13
	tbh r16
	ld	r19, 12(sp)
	ld	r19, 12(r23)

label:
	ld	r19, label(sp)
	ld	r7,(r6)
	st	r7,(r6)
	add	sp,8
	add	sp,9
	lea	r29,(sp)
	lea	r29,36(sp)
	bf	label
	nop
	ld	r3,0x1234(r6)
	ld	r3,0x1233(r6)
	ld	r3,(r6)
	ld	r3,0x0(r6)
	ld	r3,0x4(r6)
	ld	r3,0x3(r6)
	extu	r4, r6
	add	r4, r6 shl 3
	test6_add	r4, r5, r6 lsr 2
	asr	r7, 1
	asr	r7, 9
	add	r7, r8, r9
	add	r7, r8, r9 shl 8
	
	push	r0
	push	r6
	push	r16
	push	r24
	#push	r1
	