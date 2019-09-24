; BIT ROTATE - Rotate the bit pattern in register Rc
;
; Feel free to change the bit pattern below

Pattern = 0b11001100

	clra
	data Rb, Pattern
	
.begin:
	nop
	nop
	nop
	nop
	nop
	nop
	mvc Rb
	lsr
	jnc .begin
	
.addone:
	inc Rb
	jmp .begin