; Show all power series (n^1, n^2, n^3, ...) up to base 15. Limited
; to results <= 255

ADDR_BASE  = 0x00

.begin:
	clra
	data SP, 0xff       ; stack to end (255)
	inc Ra              ; set Ra to 1
	sto Ra, ADDR_BASE   ; set starting base (1)
	
.loop:
	lod Ra, ADDR_BASE  ; get the last base
	inc Ra             ; increment it
	data Rb, 0x0f      ; value to test(and) against
	and Ra             ; only interested in 1 to 15
	jz .begin          ; if we're greater than 15, start again
	sto Ra, ADDR_BASE  ; store current base in memory
	mov Rd, Ra         ; output current base to display
	mov Rb, Ra         ; store current base in Rb (Ra, Rb and Rd contain base)
	call .nextPower    ; show powers for next base
	jmp .loop          ; again!
	
.nextPower:
	data Rc, 0x00      ; reset our result to zero
	
.powerAdder:
	dec Ra             ; each iteration we need to add 'base' times
	jnn .doAdd         ; if we've already added 'base' times
	mov Rd, Rc         ; output current result to display
	mov Rb, Rc         ; set current result as the value to add next
	lod Ra, ADDR_BASE  ; reset 'base' counter
	jmp .nextPower
	
.doAdd:
	add Rc, Rb         ; iteration add current result to temp (Rc)
	jnc .powerAdder    ; if we haven't gone over 255, go again
	ret                ; finished this base