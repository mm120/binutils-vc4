.text
	nop
	
	.global delay3
	.extern delay3

	adds8 r17, 0x12345678
	vector48 0x7cd, 0x12345678
	vector80 0xcd, 0x12345678, 0x12345678
	ld r17, 0x123456(r19)
	
delay3:
	.fill 64,2,1
	b	delay3
	add	r0,r1,r2
delay4:
	.fill 65,2,1
	b	delay4
	add	r10,r11,r12
	nop

	.if 0
	
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
	; push	r1
	
	ld	r2,(sp)
	ld	r2,36(sp)

	ld	r1,(r2,r3)

	st12	r0, 0x123(r0), 0
	
	ld	r7,BIG_NUM

	.endif
	