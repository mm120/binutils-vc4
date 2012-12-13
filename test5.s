
start:	
	nop
	nop
	nop
	nop
	nop
	nop
	nop
lab:
	nop
lab2:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	bne	lab
	
	.word 0xF000 , 0xE038 , 0x0380
	.word 0xF000 , 0xE038 , 0x0380
	.word 0xF000 , 0xE038 , 0x0380
	.word 0xF000 , 0xE038 , 0x0380
	.word 0x0400
	.word 0xFC00 , 0xE038 , 0x0380 , 0xF3C0 , 0x1200
	.word 0x4000
	.word 0x005A

.if 0
0006045E: F000 E038 0380           ..8...    vector48 0x0, 0xe0380380
00060464: F000 E038 0380           ..8...    vector48 0x0, 0xe0380380
0006046A: F000 E038 0380           ..8...    vector48 0x0, 0xe0380380
00060470: F000 E038 0380           ..8...    vector48 0x0, 0xe0380380
00060476: 0400                     ..        ld r0, 0x00(sp)
00060478: FC00 E038 0380 F3C0 1200 ..8.......vector80 0x400, 0xe0380380, 0xf3c01200
00060482: 4000                     .@        mov r0, r0
00060484: 005A                     Z.        rts
00060486: B002 0020                .. .      mov r2, #0x0020
0006048A: F808 8038 03A0 F3C0 0084 ..8.......vector80 0x8, 0x803803a0, 0xf3c00084
00060494: F458 E020 0441           X. .A.    vector48 0x458, 0xe0200441
0006049A: F808 8038 0380 F3C0 0084 ..8.......vector80 0x8, 0x80380380, 0xf3c00084
000604A4: FC00 E038 0200 F3C0 0FBC ..8.......vector80 0x400, 0xe0380200, 0xf3c00fbc
000604AE: F808 8038 03C0 F3C0 0084 ..8.......vector80 0x8, 0x803803c0, 0xf3c00084
000604B8: FC00 E038 0200 F3C0 08BC ..8.......vector80 0x400, 0xe0380200, 0xf3c008bc
000604C2: F80D 8038 0380 F880 0004 ..8.......vector80 0xd, 0x80380380, 0xf8800004
000604CC: 005A                     Z.        rts
000604CE: B002 0040                ..@.      mov r2, #0x0040
000604D2: F810 C038 03C0 F3C0 0204 ..8.......vector80 0x10, 0xc03803c0, 0xf3c00204
000604DC: F458 E020 0441           X. .A.    vector48 0x458, 0xe0200441
000604E2: FC00 E038 0280 F3C0 09BC ..8.......vector80 0x400, 0xe0380280, 0xf3c009bc
000604EC: F810 C038 0380 F3C0 0204 ..8.......vector80 0x10, 0xc0380380, 0xf3c00204
000604F6: FE00 E038 0300 F3C0 0EBC ..8.......vector80 0x600, 0xe0380300, 0xf3c00ebc
00060500: F816 C038 0380 F880 0004 ..8.......vector80 0x16, 0xc0380380, 0xf8800004
0006050A: 005A                     Z.        rts
0006050C: B002 0020                .. .      mov r2, #0x0020
00060510: F88D E020 0380 23E0 0004 .. ....#..vector80 0x8d, 0xe0200380, 0x23e00004
0006051A: FC00 8038 0400 F3C0 0E80 ..8.......vector80 0x400, 0x80380400, 0xf3c00e80
00060524: F888 E020 0380 F3C0 0084 .. .......vector80 0x88, 0xe0200380, 0xf3c00084
0006052E: FC00 8038 0400 F3C0 0880 ..8.......vector80 0x400, 0x80380400, 0xf3c00880
00060538: F888 E020 03C0 F3C0 0084 .. .......vector80 0x88, 0xe02003c0, 0xf3c00084
00060542: F400 8038 0402           ..8...    vector48 0x400, 0x80380402
00060548: F500 8020 0701           .. ...    vector48 0x500, 0x80200701
0006054E: F520 8020 0502            . ...    vector48 0x520, 0x80200502
00060554: F520 8020 0604            . ...    vector48 0x520, 0x80200604
0006055A: F888 E020 03A0 F3C0 0084 .. .......vector80 0x88, 0xe02003a0, 0xf3c00084
00060564: 005A                     Z.        rts
.endif
	