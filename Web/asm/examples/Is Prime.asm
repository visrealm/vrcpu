; IS THE NUMBER PRIME?
;
; Terribly basic algorithm to find if a number is prime.
; 
; The result shown will be its largest factor
; If it's prime, the result will be 1
;
; Change the number on the first line below - is it prime?

NUMBER = 21

start:
	data Rd, NUMBER

    mov Rb, Rd
	data Ra, 7

rightShift:	; right shift
	call leftRotate
	dec Ra
	jnz rightShift
	data Ra, 0x7f
	and Ra
	mov Rb, Ra
	
	data Ra, 0x01

; check id Rc is divisible by Rb
checkNext:
	cmp Rb, Ra ; if we're down to 1
	jz .noresult
	
	mov Rc, Rd
	.doSubtract:
		sub Rc
		jz .result
		jc .doSubtract
		dec Rb
		jmp checkNext

	.result:
		mov Rd, Rb
		hlt

	.noresult:
		mov Rd, 1
		hlt
	
leftRotate:
	lsr
	jnc .ret
	.lsaddone:
		inc Rb
	.ret:
		ret