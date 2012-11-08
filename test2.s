.text
	nop
	
	.global delay3
	.extern delay3
delay3:
	nop
	nop
	b	delay3
	