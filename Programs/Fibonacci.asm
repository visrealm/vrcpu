; FIBONACCI SEQUENCE
;
; If you want to halt instead of repeat, look
; at the jc .begin line. You might want to create
; Another label with another action to end ;)

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
