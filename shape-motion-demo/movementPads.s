	.arch msp430g2553
	
	.data
	.global swSD1		;switch 1
swSD1:	.byte 0x00		;0
	
	.data
	.global swSD2		;switch 2
swSD2:	.byte 0x00		;0

	.data
	.global swSD3		;switch 3
swSD3:	.byte 0x00		;0

	.data
	.global swSD4		;switch 4
swSD4:	.byte 0x00		;0

	.data
	.global bit0		;BIT0
bit0:	.byte 0x01
	
	.data
	.global bit1		;BIT1
bit1:	.byte 0x02

	.data
	.global bit2		;BIT2
bit2:	.byte 0x04

	.data
	.global bit3		;BIT3
bit3:	.byte 0x08
	
	
	.text
	.global movePlayers
movePlayers:

	bit.b &bit0, &P2IN	;(P2IN & BIT0)
	mov #0, &swSD1		;if S1 is not press make equal to 0
	jnz ifS2press		;button S1 not press check next button
	mov #1, &swSD1 		;button S1 press
	call #P1MovUp		;call the move up method for P1
	jmp ifS2press		;go to next button 

ifS2press:
	
	bit.b &bit1, &P2IN	;(P2IN & BIT1)
	mov #0, &swSD2		;if S2 is not press make equal to 0
	jnz ifS3press		;button S2 not press check next button end
	mov #1, &swSD2 		;button S2 press make equal to 1
	call #P1MovDown		;;call the move down method for P1
	jmp ifS3press		;go to next button 

ifS3press:

	bit.b &bit2, &P2IN	;(P2IN & BIT2)
	mov #0, &swSD3		;if S3 is not press make equal to 0
	jnz ifS4press		;button S3 not press check next button
	mov #1, &swSD3 		;button S3 press
	call #P2MovUp		;call the move up method for P2
	jmp ifS4press		;go to next button

ifS4press:
	
	bit.b &bit3, &P2IN	;(P2IN & BIT3)
	mov #0, &swSD4		;if S4 is not press make equal to 0
	jnz done		;button S4 not press check next button end
	mov #1, &swSD4 		;button S4 press make equal to 1
	call #P2MovDown		;;call the move down method for P2
	jmp done		;go to end 



done:	pop 0
