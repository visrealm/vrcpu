; BOUNCE - bounce between 0 and 255

	clra
	
.begin:	
	mov Rd, Ra
	inc Ra
	jnz .begin
	dec Ra
	
.loop:
	mov Rd, Ra
	dec Ra
	jnz .loop
	jmp 0
