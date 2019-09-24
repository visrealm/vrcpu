; FIBONACCI

.begin:	
	clra
	inc Rb
	
.loop:
	add Rc, Rb
	jc .begin
	mov Rd, Rc
	mov Rc, Rb
	mov Rb, Rd
	jmp .loop
