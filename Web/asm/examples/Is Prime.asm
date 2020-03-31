; IS THE NUMBER PRIME?
;
; Terribly basic algorithm to find if a number is prime.
; 
; The result shown will be its largest factor
; If it's prime, the result will be 1
;
; Change the number on the first line below - is it prime?

NUMBER = 57

start:
	data Rd, NUMBER

    mov Rb, Rd
	data Ra, 7

.lsr:	; right shift
	call .ls
	dec Ra
	jnz .lsr
	
	inc Rb
	jnn .next
	data Rc, 0x80 ; remove high bit if present
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
	jnc .next
	jmp .sub
	
.noresult:
	mov Rd, 1
	hlt
	
.ls:
	lsr
	jnc ret
.lsaddone:
	inc Rb

ret:
	ret