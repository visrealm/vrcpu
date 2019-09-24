; IS THE NUMBER PRIME?
; If it's prime, the rersult will be 1 (largest number divisble)

	data Rd, #235

    mov Rb, Rd
	data Ra, #7
	data Rc, .ls

.lsr:	; right shift
	call
	dec Ra
	jnz .lsr
	
	inc Rb
	jnn .next
	data Rc, #128 ; remove high bit if present
	sub Rb, Rc
	mov Rb, Rc

.next:
	dec Rb
	jnz .next2
	jmp .noresult
	
.next2:
	mov Rc, Rd
.sub:
	sub Rc
	jnz .test
	
.result:
	mov Rd, Rb
	hlt
	
.test:
	jc .next
	jmp .sub
	
.noresult:
	mov Rd, #1
	hlt
	
.ls:
	lsr
	jnc .ret
.lsaddone:
	inc Rb
.ret:
	ret