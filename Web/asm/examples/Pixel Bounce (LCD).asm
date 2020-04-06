; Bounce a pixel around the display
; Note: The emulator isn't fast enough to run this one
;        It looks much more convincing on the real device

DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON

_PIXELX = 0x00  ; pixel overall position
_PIXELY = 0x01  

_CHARX = 0x02   ; character the pixel is at
_CHARY = 0x03

_PIXEL_OFFSETX = 0x04  ; location within the character
_PIXEL_OFFSETY = 0x05

_LAST_CHARX = 0x06   ; last character position
_LAST_CHARY = 0x07

_DIR_X = 0x08
_DIR_Y = 0x09

CHAR_WIDTH = 5
CHAR_HEIGHT = 8

PIXEL_POS_X = 0
PIXEL_POS_Y = 0

initializeDisplay:
	clra
	lcc #LCD_INITIALIZE
	lcc #DISPLAY_MODE
	lcc #LCD_CMD_CLEAR
	
	sto Ra, _LAST_CHARX ; clear lastChar positions
	sto Ra, _LAST_CHARY
	
	inc Ra
	sto Ra, _DIR_Y
	inc Ra
	sto Ra, _DIR_X ; movement direction

	
	; set pixel location
	data Ra, PIXEL_POS_X
	sto Ra, _PIXELX
	data Ra, PIXEL_POS_Y
	sto Ra, _PIXELY
	
	call clearChar	
	
drawPixel:

checkXBounds:
	lod Ra, _PIXELX
	tst Ra
	jn .fixNeg
	data Rb, 79
	cmp Rb, Ra
	jnn .done
	
	.fixPos:
		data Ra, 79
		sto Ra, _PIXELX		
		jmp .reverse
	
	.fixNeg:
		data Ra, 0
		sto Ra, _PIXELX		
	
	.reverse:
			lod Rb, _DIR_X
			data Rc, 0
			sub Rc
			mov Rb, Rc
			sto Rb, _DIR_X
			inc Rd
	.done:

checkYBounds:
	lod Ra, _PIXELY
	tst Ra
	jn .fixNeg
	data Rb, 15
	cmp Rb, Ra
	jnn .done
	
	.fixPos:
		data Ra, 15
		sto Ra, _PIXELY
		jmp .reverse
	
	.fixNeg:
		data Ra, 0
		sto Ra, _PIXELY
	
	.reverse:
			lod Rb, _DIR_Y
			data Rc, 0
			sub Rc
			mov Rb, Rc
			sto Rb, _DIR_Y
			inc Rd
	.done:

			
	; compute X location and offset
	data Rb, CHAR_WIDTH
	lod Ra, _PIXELX
	call div8
	sto Rc, _CHARX
	sto Ra, _PIXEL_OFFSETX
	
	; compute Y location and offset
	data Rb, CHAR_HEIGHT
	lod Ra, _PIXELY
	call div8
	sto Rc, _CHARY
	sto Ra, _PIXEL_OFFSETY
	
	call clearLastCharacter	
	
	; Set CGRAM address to _PIXEL_OFFSETY
	lod Ra, _PIXEL_OFFSETY
	data Rb, LCD_CMD_SET_CGRAM_ADDR
	or Ra
	lcc Ra
	
	data Ra, 4
	lod Rb, _PIXEL_OFFSETX	
	sub Ra
	data Rb, 0x01
	tst Ra
	jz .donePixelShift
	.checkShift:
		lsr
		dec Ra
		jnz .checkShift

.donePixelShift:
	lcd Rb ; update the custom character with the pixel location
	
	call outputCharacter
	
	lod Ra, _PIXELX
	lod Rb, _DIR_X
	add Ra
	sto Ra, _PIXELX
	lod Ra, _PIXELY
	lod Rb, _DIR_Y
	add Ra
	sto Ra, _PIXELY
	jmp drawPixel
	
outputCharacter:	
	lod Ra, _CHARX
	sto Ra, _LAST_CHARX
	lod Rb, _CHARY
	sto Rb, _LAST_CHARY
	tst Rb
	jz .doneOffset
		data Rb, 0x40
		or Ra
	.doneOffset:
	data Rb, LCD_CMD_SET_DRAM_ADDR
	add Ra
	lcc Ra  ; set the dram location
	lcd #0  ; output the character
	ret
	
clearLastCharacter:
	lod Ra, _LAST_CHARX
	lod Rb, _LAST_CHARY
	tst Rb
	jz .doneOffset
		data Rb, 0x40
		or Ra
	.doneOffset:
	data Rb, LCD_CMD_SET_DRAM_ADDR
	add Ra
	lcc Ra  ; set the dram location
	lcd #32  ; output space
	call clearChar ; clear the last custom character data too
	ret
	
clearChar:
	lcc #LCD_CMD_SET_CGRAM_ADDR
	data Rb, CHAR_HEIGHT
	data Ra, 0
	.next:
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
	data Rc, 0x00
	
	.step:
		cmp Rb, Ra
		jz .add
		jc .return
		
	.add:
		inc Rc
		sub Ra
		jnz .step

	.return:
		ret