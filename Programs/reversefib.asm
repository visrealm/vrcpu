; REVERSE FIBONACCI

begin:
	data Rb, #144
	data Rd, #233
	
loop:	
	mov Rc,Rd
	sub Rc,Rb
	mov Rd,Rb
	mov Rb,Rc
	jnz loop
	
next:	
	mov Rd,Rb
	jmp begin