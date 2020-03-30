; FACTORIALS
;
; Cycles through factorial numbers (0! -> 5!)

.begin:
	data Rd, 1
	data Ra, 0
	push Ra
	inc Ra

.next:
	pop Rc
	inc Rc
	push Rc
	mov Rd, Ra
	mov Rb, Ra

.mul:
	add Ra
	jc .end
	dec Rc
	jz .next
	jmp .mul

.end:
	pop Rc
	jmp .begin