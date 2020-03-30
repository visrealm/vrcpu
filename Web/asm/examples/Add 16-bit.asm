; Add two 16 bit numbers.
; Place result high byte in Rc, low byte in Rb
; Carry flag will be set if result is > 16 bits

FIRST =  0x00
SECOND = FIRST + 0x02

NUMBER1 = 12345
NUMBER2 = 12122

.setup:
	clra
	data SP, 0xff
	
	; first number
	data Ra, NUMBER1 >> 8  ; high byte
	data Rd, NUMBER1       ; low byte
	sto Rd, FIRST
	sto Ra, FIRST + 0x01

	; second number
	data Ra, NUMBER2 >> 8  ; high byte
	data Rd, NUMBER2       ; low byte
	sto Rd, SECOND
	sto Ra, SECOND + 0x01
	
	data Ra, FIRST
	data Rd, SECOND
	call .add16
	
	lod Rd, FIRST
	lod Rc, FIRST + 0x01
	data Rb, 0xff
	add Ra ; ensure the carry flag is set if we overflowed
	mov Rb, Rd
		
	hlt

; adds 16-bit numbers in memory. 
; Number 1 address in Ra
; Number 2 address in Rd
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