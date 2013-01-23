	.fill 7, 2, 1
lab:
	.fill (0x80 / 2)-7, 2, 1
	bne	lab
	
