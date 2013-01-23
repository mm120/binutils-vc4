
.text
	and r0, 0x20
	cmp r0, 0
	cmp r0, 1
	cmp r0, 0x1f
	cmp r0, 0x20
	cmp r0, -1
	cmp r0, -0x1f
	cmp r0, -0x20
	cmp r0, 32767
	cmp r0, 32768
	cmp r0, 32769
	cmp r0, -32767
	cmp r0, -32768
	cmp r0, -32769
	cmp r0, 0xfffffffe
	cmp r0, 0x87654321
