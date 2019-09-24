; BIT SHIFT - Bit shift the pattern in Rb

	clra
	data Rb, 0b11001100
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