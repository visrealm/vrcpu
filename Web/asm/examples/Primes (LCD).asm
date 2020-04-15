; IS THE NUMBER PRIME?
;
; Terribly basic algorithm to find if a number is prime.
; 
; The result shown will be its largest factor
; If it's prime, the result will be 1
;
; Change the number on the first line below - is it prime?

NUMBER = 228

DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON
NEXTLINE = LCD_CMD_SET_DRAM_ADDR | 40

lcc #LCD_INITIALIZE
lcc #DISPLAY_MODE

ADDR   = 0
STR_TERM = 0
BCD_TERM = 255
ASCII_ZERO = 48 ; ascii 0

	clra
	clr Rd
	push Rd
; compute if Rd is prime
start:
	lcc #LCD_CMD_CLEAR
	data SP, 255
	pop Rd
	inc Rd
	push Rd
	
	mov Rb, Rd
	call printNumber
	data Ra, strIs
	call printStr
	
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
	push Rb
	dec Rb
	jz .noresult
	data Ra, strNot
	call printStr
	data Ra, strPrime
	call printStr
	peek Rb
	data Ra, ADDR
	call toDec8
	lcc #NEXTLINE
	data Ra, strDivisibleBy
	call printStr
	pop Rb
	data Ra, ADDR
	call printBCD
	pop Ra ; lose current result
	clr Ra
	.pause: ; little loop to hold the answer for a bit
		dec Ra
		jz start
		jmp .pause
	
.test:
	jnc .next
	jmp .sub
	
.noresult:
	data Ra, strPrime
	call printStr
	clr Ra
	.pause2: ; little loop to hold the answer for a bit
		dec Ra
		jz start
		jmp .pause2
	jmp start
	
.ls:
	lsr
	jnc ret
.lsaddone:
	inc Rb

ret:
	ret
	
printStr:
  mov Rc, Ra
  .nextChar:
    lod Ra, Rc
    tst Ra
    jz ret
    lcd Ra
	inc Rc
    jmp .nextChar

; output number in Rb to display
printNumber:
	data Ra, ADDR
	call toDec8
	data Ra, ADDR
	call printBCD
	ret

; print BCD value at address in Ra
printBCD:
	data Rb, BCD_TERM
	
	.findEnd:
		lod Rc, Ra
		cmp Rb, Rc
		jz .startPrint
		inc Ra
		jmp .findEnd

	.startPrint:
		clr Rc
		;inc Ra
	
	.nextDigit:
		dec Ra
		lod Rc, Ra
		data Rb, ASCII_ZERO
		add Rc
		lcd Rc
		mov Rb, Ra
		tst Rb
		jz ret
		jmp .nextDigit
		ret


; function toDec - binary to decimal
;  Rb: number
;  Ra: memory address to store result. STR_TERM delimited
toDec8:
	push Ra
	push Rb
	
	.nextDigit:
		pop Ra  ; get remaining number
		data Rb, 10
		call div8
		pop Rb  ; address to Rb
		sto Rb, Ra ; store remainder
		inc Rb
		push Rb  ; push next memory address
		tst Rc
		jz .return
		push Rc
		jmp .nextDigit

	.return:
		pop Rb
		data Ra, BCD_TERM  ; add terminator
		sto Rb, Ra
		ret

; function div8 - divide two 8-bit integers
;  Ra: dividend
;  Rb: divisor
; returns:
;  Rc: result
;  Ra: remainder
div8:
	data Rc, 0x00
	
	.step:
		cmp Rb, Ra
		jz .add
		jc ret
		
	.add:
		inc Rc
		sub Ra
		jnz .step
		ret

strIs: #str " is \0"	
strNot: #str "not \0"
strPrime: #str "prime\0"
strDivisibleBy: #str "Divisible by \0"