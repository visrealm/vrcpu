; Bounce a pixel around the display

DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON

_PIXELX = 0x00  ; pixel overall position
_DIR_X = _PIXELX + 1

_PIXELY = 0x02
_DIR_Y = _PIXELY + 1  

_CHARX = 0x04   ; character the pixel is at
_CHARY = 0x05

_PIXEL_OFFSETX = 0x06  ; location within the character
_PIXEL_OFFSETY = 0x07

_LAST_CHARX = 0x08   ; last character position
_LAST_CHARY = 0x09

_BALL_IMAGES = 0x40 ; end address for ball images

CHAR_WIDTH = 5
CHAR_HEIGHT = 8

PIXEL_POS_X = 8
PIXEL_POS_Y = 0

BALL_HEIGHT = 4
BALL_WIDTH = 4

SCREEN_WIDTH = 80
SCREEN_HEIGHT = 16

PLAYABLE_WIDTH = SCREEN_WIDTH - BALL_WIDTH
PLAYABLE_HEIGHT = SCREEN_HEIGHT - BALL_HEIGHT


clra
data SP, 255
jmp start

; ball at each of the 6 horizontal locations
; there is an empty space between character cells

ballData:
#d32 0x0c1e0000
#d32 0x060f0000
#d32 0x03070000
#d32 0x01030010
#d32 0x00011018
#d32 0x0000181c

start:



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

	data Rb, PLAYABLE_WIDTH - 1
	data Ra, _PIXELX
	call checkBounds

	data Rb, PLAYABLE_HEIGHT - 1
	data Ra, _PIXELY
	call checkBounds
			
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
	
	mov Rb, ballData
	lod Ra, Rb
	lcd Ra
	inc Rb
		lod Ra, Rb
	lcd Ra
	inc Rb
		lod Ra, Rb
	lcd Ra
	inc Rb
		lod Ra, Rb
	lcd Ra
	inc Rb
	
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