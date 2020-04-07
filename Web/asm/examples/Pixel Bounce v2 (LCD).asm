; Bounce a pixel around the display

DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON

_PIXELX = 0x00  ; pixel overall position
_DIR_X = _PIXELX + 1

_PIXELY = 0x02
_DIR_Y = _PIXELY + 1  

_CHARX = 0x04   ; character the pixel is at

_PIXEL_OFFSETX = 0x06  ; location within the character

_LAST_CHARX = 0x08   ; last character position

_BALL_IMAGES = 0x40 ; end address for ball images

_CHARSET = 0x0a ; // character set (0 or 4)

CHAR_WIDTH = 5
CHAR_HEIGHT = 8

BALL_HEIGHT = 4
BALL_WIDTH = 4

SCREEN_WIDTH = 80
SCREEN_HEIGHT = 16

PLAYABLE_WIDTH = SCREEN_WIDTH - BALL_WIDTH
PLAYABLE_HEIGHT = SCREEN_HEIGHT - BALL_HEIGHT

NEXTLINE = 0x40 | LCD_CMD_SET_DRAM_ADDR



	clra
	clr SP

initializeDisplay:
	lcc #LCD_INITIALIZE
	lcc #DISPLAY_MODE
	lcc #LCD_CMD_CLEAR
	
	sto Ra, _PIXELY
	
	inc Ra
	sto Ra, _PIXELX
	sto Ra, _DIR_Y
	sto Ra, _LAST_CHARX ; ensure last char positions are.. different (1)
	
	inc Ra
	sto Ra, _DIR_X ; movement direction

drawPixel:

	data Rb, PLAYABLE_WIDTH - 1
	data Ra, _PIXELX
	call checkBounds

	data Rb, PLAYABLE_HEIGHT
	data Ra, _PIXELY
	call checkBounds
	
	; compute X location and offset
	data Rb, CHAR_WIDTH
	lod Ra, _PIXELX
	call div8
	sto Rc, _CHARX
	sto Ra, _PIXEL_OFFSETX

	call determineCharset
	
	lod Ra, _PIXELY
	
	call setCgRamAddr
	
	mov Ra, ballData
	
	call updateChar
	
	; Set CGRAM address to _PIXEL_OFFSETY
	lod Ra, _PIXELY
	data Rb, 16
	add Ra
	
	call setCgRamAddr
	
	mov Ra, ballData + 2
	
	call updateChar
	
	call outputCharacters

	lod Ra, _PIXELX
	lod Rb, _DIR_X
	add Ra
	sto Ra, _PIXELX
	lod Ra, _PIXELY
	lod Rb, _DIR_Y
	add Ra
	sto Ra, _PIXELY
	jmp drawPixel


; set the cgram address. if it's not 0, then
; go back one more and clear it too
setCgRamAddr:
	lod Rb, _CHARSET
	lsr
	lsr
	lsr
	clr Rc
	add Ra
	jz .nosub
	inc Rc
	dec Ra
	.nosub:
		data Rb, LCD_CMD_SET_CGRAM_ADDR
		add Ra
		lcc Ra
	tst Rc
	jz .ret
	lcd #0
	.ret:
	ret

updateChar:
	lod Rb, _PIXEL_OFFSETX
	lsr
	lsr
	add Ra
	mov Rc, Ra
	lod Rb, Rc
	inc Rc
	lod Ra, Rc
	lcd Rb
	lcd Ra
	lcd Ra
	lcd Rb
	lcd #0 ; clear the last line
	ret

checkBounds:
	push Ra
	lod Ra, Ra
	tst Ra
	jn .fixNeg
	cmp Rb, Ra
	jnn .done
	
	.fixPos:
		jmp .reverse
	
	.fixNeg:
		clr Rb
	
	.reverse:
			pop Ra
			sto Ra, Rb
			inc Ra ; direction
			lod Rb, Ra
			clr Rc
			sub Rc
			sto Ra, Rc
			inc Rd
			ret
	.done:
	pop Ra
	ret

; sets to 0 if _CHARX is even, 4 if it's odd
determineCharset:	
	data Rb, 1
	lod Ra, _CHARX
	and Ra
	mov Rb, Ra
	lsr
	lsr
	lod Ra, _CHARSET
	cmp Ra
	jz .ret
	sto Rb, _CHARSET
	call clearChars
	.ret:
	ret

outputCharacters:	
	lod Rb, _LAST_CHARX
	lod Ra, _CHARX
	cmp Ra
	jz .ret

	sto Ra, _LAST_CHARX

	clr Rc
	tst Ra
	jz .doOutput
	dec Rc
	
	.doOutput:
	sto Ra, _LAST_CHARX
	data Rb, LCD_CMD_SET_DRAM_ADDR
	add Ra
	mov Rb, Rc
	add Ra
	lcc Ra  ; set the dram location

	push Rc
	mov Ra, Rc
	lod Rb, _CHARSET
	call .outputChars

	data Rb, NEXTLINE
	lod Ra, _CHARX
	add Ra
	peek Rb
	add Ra
	lcc Ra  ; set the dram location
	lod Rb, _CHARSET
	inc Rb
	pop Ra
	call .outputChars


	.ret:
		ret
		
	.outputChars:
		tst Ra
		jz .skipSpace1
		lcd #32
		.skipSpace1:
		lcd Rb
		inc Rb
		inc Rb
		lcd Rb
		lcd #32
		ret
			
	
clearChars:
	clr Ra
	call setCgRamAddr
	clr Ra	
	data Rb, 8
	.next:
	  lcd Ra
	  lcd Ra
	  lcd Ra
	  lcd Ra
	  dec Rb
	  jnz .next
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
		
		; ball at each of the 6 horizontal locations
; there is an empty space between character cells

ballData:
; empty ball
;#d32 0x0c120000
;#d32 0x06090000
;#d32 0x03080000
;#d32 0x01020010
;#d32 0x00011008

; filled ball
#d32 0x0c1e0000
#d32 0x060f0000
#d32 0x03070000
#d32 0x01030010
#d32 0x00011018
;#d32 0x0000181c
