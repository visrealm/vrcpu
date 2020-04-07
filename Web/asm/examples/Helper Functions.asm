

; function abs8 - return absolute value of 8-bit number
;  Ra: number
; returns:
;  Ra: result
abs8:
	tst Ra
	jnn .return
	not Ra
	dec Ra
	
	.return:
		ret

; function mul8 - divide two 8-bit integers
;  Ra: num1
;  Rb: num2
; returns:
;  Rc: result low
;  Rd: result high
mul8:
	data Rc, 0x00
	data Rd, 0x00
	
	.step:
		tst Ra
		jz .return
		
	.add:
		add Rc
		jc .addHigh
		
	.cont:
		dec Ra
		jmp .step
		
	.addHigh:
		inc Rd
		jmp .cont		

	.return:
		ret
		
; function div8 - divide two 8-bit integers
;  Ra: dividend
;  Rb: divisor
; returns:
;  Rc: result
;  Ra: remainder
div8:
	clr Rc
	
	.step:
		cmp Ra, Rb
		jnc .return
		
	.add:
		inc Rc
		sub Ra
		jnz .step

	.return:
		ret

		
; function add16 - adds 16-bit numbers in memory. 
; Number 1 address in Ra
; Number 2 address in Rd
; Result is placed back in memory at address in Ra. If result
; overflows, it will be flagged in Ra
.add16:
	lod Rb, Ra ; load first low byte into Rb
	lod Rc, Rd ; load second low byte into Rc
	add Rc     ; sum to Rc
	sto Ra, Rc ; write back to first address
	jnc .addHigh
	
.addCarry:
	inc Ra
	lod Rb, Ra
	inc Rb
	sto Ra, Rb
	dec Ra
	
.addHigh:
	inc Ra   ; go to high byte
	inc Rd   ; go to high byte
	lod Rb, Ra ; load first low byte into Rb
	lod Rc, Rd ; load second low byte into Rc
	add Rc     ; sum to Rc
	sto Ra, Rc ; write back to first address
	data Ra, 0
	jnc .done
	inc Ra ; overflow
	
.done:
	ret