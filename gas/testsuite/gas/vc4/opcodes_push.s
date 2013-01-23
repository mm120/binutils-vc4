

	.text

	push	r6
	pop	r6
	push	r6, lr
	pop	r6, pc
	push	r0
	push	r16
	push	r24
	pop	r0
	pop	r16
	pop	r24
	push	r0, lr
	push	r16, lr
	push	r24, lr
	pop	r0, pc
	pop	r16, pc
	pop	r24, pc
	
	push	r6-r5
	push	r6-r6
	push	r6-r7
	push	r6-r0
	push	r6-r31
	push	r6-r5, lr
	push	r6-r6, lr
	push	r6-r7, lr
	push	r6-r0, lr
	push	r6-r31, lr
	
	pop	r6-r5
	pop	r6-r6
	pop	r6-r7
	pop	r6-r0
	pop	r6-r31
	pop	r6-r5, pc
	pop	r6-r6, pc
	pop	r6-r7, pc
	pop	r6-r0, pc
	pop	r6-r31, pc

	push	r0-r5
	push	r0-r6
	push	r0-r7
	push	r0-r0
	push	r0-r31
	push	r0-r5, lr
	push	r0-r6, lr
	push	r0-r7, lr
	push	r0-r0, lr
	push	r0-r31, lr
	
	pop	r0-r5
	pop	r0-r6
	pop	r0-r7
	pop	r0-r0
	pop	r0-r31
	pop	r0-r5, pc
	pop	r0-r6, pc
	pop	r0-r7, pc
	pop	r0-r0, pc
	pop	r0-r31, pc

	push	r16-r5
	push	r16-r6
	push	r16-r7
	push	r16-r0
	push	r16-r31
	push	r16-r5, lr
	push	r16-r6, lr
	push	r16-r7, lr
	push	r16-r0, lr
	push	r16-r31, lr
	
	pop	r16-r5
	pop	r16-r6
	pop	r16-r7
	pop	r16-r0
	pop	r16-r31
	pop	r16-r5, pc
	pop	r16-r6, pc
	pop	r16-r7, pc
	pop	r16-r0, pc
	pop	r16-r31, pc

	push	r24-r5
	push	r24-r6
	push	r24-r7
	push	r24-r0
	push	r24-r31
	push	r24-r5, lr
	push	r24-r6, lr
	push	r24-r7, lr
	push	r24-r0, lr
	push	r24-r31, lr
	
	pop	r24-r5
	pop	r24-r6
	pop	r24-r7
	pop	r24-r0
	pop	r24-r31
	pop	r24-r5, pc
	pop	r24-r6, pc
	pop	r24-r7, pc
	pop	r24-r0, pc
	pop	r24-r31, pc

	# These are really "st r1,(r25)--", "ld r1,(r25)++" (and are two words each!)
	push	r1
	push	r3
	pop	r2
	pop	r7
	