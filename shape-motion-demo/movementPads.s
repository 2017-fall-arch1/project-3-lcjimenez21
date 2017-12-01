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
bit0:	.byte 0x00

	.data
	.global bit0		;BIT1
bit1:	.byte 0x02

	.data
	.global bit0		;BIT2
bit2:	.byte 0x04

	.data
	.global bit0		;BIT3
bit3:	.byte 0x08
	
	
	.text
	.global movePlayer
movePlayer:

	cmp.b &P2IN, &bit0 	;(P2IN & BIT0)
	jz ifS2press		;button S1 not press check next button
	mov #1, &swSD1		;button S1 press
	jmp endSwitch		;go to end of comparison

ifS2press:
	cmp.b &P2IN, &bit1	;(P2IN & BIT1)
	jz ifS3press		;button S2 not press check next button
	mov #1, &swSD2		;button S2 press
	jmp endSwitch		;go to end of comparison


ifS3press:
	cmp.b &P2IN, &bit2	;(P2IN & BIT2)
	jz ifS4press		;button S3 not press check next button
	mov #1, &swSD3		;button S3 press
	jmp endSwitch		;go to end of comparison


	
ifS4press:
	cmp.b &P2IN, &bit3	;(P2IN & BIT3)
	jz endSwitch		;No button was px2_ress
	mov #1, &swSD4		;button S4 press


endSwitch:	
	
	cmp.b #1, &swSD1	;check that S1 was press
	jnz if2 		;if not check next button
	call #P1MovUp		;call method to move the Player1 Up
	ret 
	
if2:	cmp.b #1, &swSD2	;check that S2 was press
	jnz if3 		;if not check next button
	call #P1MovDown		;call method to move the Player2 Down
	ret 

if3:	cmp.b #1, &swSD3	;check that S3 was press
	jnz if4 		;if not check next button
	call #P2MovUp		;call method to move the Player2 Up
	ret 

if4:	cmp.b #1, &swSD4	;check that S4 was press
	jnz  done		;if no bottom was press your done
	call #P2MovDown		;call method to move the Player2 Down

done:	ret 
