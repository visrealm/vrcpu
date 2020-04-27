; ===============================================
; Snake!
; -----------------------------------------------
;
; Troy's 8-bit computer - Emulator
;
; Copyright (c) 2020 Troy Schrapel
;
; This code is licensed under the MIT license
;
; https://github.com/visrealm/vrcpu
;
; ===============================================

DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON
NEXTLINE = LCD_CMD_SET_DRAM_ADDR | 0x40
SCORE_POS = NEXTLINE + 12

CGRAM_START = LCD_CMD_SET_CGRAM_ADDR
CGRAM_END = CGRAM_START + 8 * 8 - 1

BTN_LEFT = 0b01000000
BTN_DOWN = 0b00100000
BTN_RIGHT = 0b00010000
BTN_UP  = 0b10000000


ASCII_ZERO = 48

; RAM locations for variables
_SCORE_ASCII           = 0x00
_APPLE                 = _SCORE_ASCII + 2
_NEXT_APPLE            = _APPLE + 1

_SNAKE_DIR             = _NEXT_APPLE + 1
_SNAKE_HEAD            = _SNAKE_DIR + 1
_SNAKE_TAIL            = _SNAKE_HEAD + 1

_PIXEL_DIFF             =_SNAKE_TAIL + 1

GAME_AREA_X            = 16
GAME_AREA_Y            = 14

DIR_UP                 = -1
DIR_DOWN               = 1
DIR_RIGHT              = 16
DIR_LEFT               = -16


; start of the program
start:
	lcc #LCD_INITIALIZE
	lcc #DISPLAY_MODE
	lcc #LCD_CMD_CLEAR

	clra
	data Ra, ASCII_ZERO
	sto Ra, _SCORE_ASCII
	sto Ra, _SCORE_ASCII + 1
	
	call buildCustomCharacters
	call drawGameScreen

	data Rb, 0b01001000 ; start location
	push Rb
	call drawPixelLoc
	pop Rb
	data Ra, snakeBuffer
	sto Ra, Rb
	sto Ra, _SNAKE_HEAD ; head and tail 
	sto Ra, _SNAKE_TAIL
	data Rc, DIR_DOWN
	sto Rc, _SNAKE_DIR	

gameLoop:
	mov Rb, Rd
	lod Ra, _NEXT_APPLE
	add Ra
	inc Ra
	sto Ra, _NEXT_APPLE
	data Ra, controlMap - 1
.nextButtonTest:
	inc Ra
	lod Rc, Ra
	tst Rc
	jz .endControlTest
	inc Ra
	and Rc
	jz .nextButtonTest
	lod Ra, Ra
	sto Ra, _SNAKE_DIR
	
.endControlTest:	
	
	lod Rb, _APPLE
	push Rb
	call drawPixelLoc
	
	lod Rb, _SNAKE_HEAD
	push Rb
	lod Rb, Rb
	
	
	lod Rc, _SNAKE_DIR
	add Rc
	;jc .die
	peek Rb
	call incrementSnakeBuffer
	sto Rb, _SNAKE_HEAD
	sto Rb, Rc

	mov Rb, Rc
	call drawPixelLoc
	lod Rb, _APPLE
	call drawPixelLoc

	pop Rb
	lod Rb, Rb
	pop Rc
	cmp Rc
	jnz .incrementTail
	call incrementScore
	lod Rb, _NEXT_APPLE
	data Rc, 0xfd
	and Rc
	sto Rc, _APPLE
	jmp .endIncrementTail
.incrementTail:
	lod Rb, _SNAKE_TAIL
	push Rb
	lod Rb, Rb
	call drawPixelLoc
	pop Rb
	call incrementSnakeBuffer
	sto Rb, _SNAKE_TAIL
.endIncrementTail:

.updateScore:
	lcc #SCORE_POS
	lod Ra, _SCORE_ASCII + 1
	lcd Ra
	lod Ra, _SCORE_ASCII
	lcd Ra

	jmp gameLoop
.die:	
;	hlt
	
incrementSnakeBuffer:
	inc Rb
	data Ra, snakeBufferEnd
	cmp Ra
	jnz .ret
	data Rb, snakeBuffer
.ret:	
	ret

drawPixelLoc:
	call getXY
; flow on through

; Rb = x pos
; Ra = y pos
drawPixelXy:
	inc Ra
	push Ra
	data Rc, bitOffsets
	add Rc
	mov Rb, Rc
	lod Rb, Rb      ; load the bitOffset
	data Rc, 0x30
	and Rc
	mov Ra, Rb ; Ra now holds full Bitoffset byte
	pop Rb ; add Y offset
	push Ra
	add Rc
	mov Rb, Rc
	data Rc, screenBuffer
	add Rc   ; Rc is screenBuffer byte
	data Ra, CGRAM_START
	or Ra    ; Ra is CGRAM command
	lcc Ra
	pop Ra
	data Rb, 0x07
	and Ra   ; Ra is number of bit shifts
	data Rb, 1
.again:	
	dec Ra
	jz .done
	lsr
	jmp .again
.done:
	mov Ra, Rb ; Ra now contains pixel set 
	mov Rb, Rc
	lod Rb, Rb ; load current buffer byte
	xor Ra
	mov Rb, Rc
	sto Rb, Ra  ; update buffer
	lcd Ra      ; output to lcd
	ret

	

; Rb = pos
; returns:
;   Rb: X
;   Ra: Y
getXY:
	data Rc, 0xf0
	data Ra, 0x0f
	and Ra
	and Rc
	data Rb, GAME_AREA_Y
	cmp Ra
	jn .fixed
	clr Ra
.fixed:
	mov Rb, Rc
	data Rc, leftRotate
	call
	call
	call
	
	; flow through to leftRotate
	
; leftRotate()
; bitwise rotate Rb
leftRotate:
	lsr
	jnc .done
	inc Rb
.done:
	ret
	


drawGameScreen:
	lcc #LCD_CMD_SET_DRAM_ADDR
	data Ra, (gameScreen - RAM_OFFSET)
	data Rb, 16
.line1:	
	lod Rc, Ra
	lcd Rc
	inc Ra
	dec Rb
	jnz .line1
	lcc #NEXTLINE
	data Rb, 10
.line2:
	lod Rc, Ra
	lcd Rc
	inc Ra
	dec Rb	
	jnz .line2
	ret

incrementScore:
	lod Ra, _SCORE_ASCII
	inc Ra
	data Rb, ASCII_ZERO + 10
	cmp Ra
	jnz .ret
	lod Ra, _SCORE_ASCII + 1
	inc Ra
	sto Ra, _SCORE_ASCII + 1	
	data Ra, ASCII_ZERO
.ret:
	sto Ra, _SCORE_ASCII
	ret
	
; write the custom characters to the LCD CGRAM
buildCustomCharacters:	
	data Ra, CGRAM_START
	lcc Ra
	data Rb, (charData)
	data Ra, (charDataEnd)
	push Ra
.addLine:
	lod Rc, Rb
	sto Ra, Rc
	lcd Rc
	inc Ra
	inc Rb
	peek Rc
	cmp Rc
	jnz .addLine
	pop Rc
	ret
; end buildCustomCharacters()


#addr RAM_OFFSET + _APPLE

#d8 0b01100100

#addr RAM_OFFSET + 10

; ----------------------------
; custom character definitions
; ----------------------------
; these are mapped at LCD code 0 - 3. 
; then vertically flipped at 7 - 4. 
; 0 is referenced using 8 to avoid
; confusion with a null terminator
charData: 
#d8 0b01111
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000   

#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01111

#d8 0b11111
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000

#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b11111

#d8 0b11111
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000

#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b11111

#d8 0b11110
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010

#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b11110
charDataEnd:

screenBuffer:
#d8 0b01111
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000   

#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01000
#d8 0b01111

#d8 0b11111
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000

#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b11111

#d8 0b11111
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000

#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b11111

#d8 0b11110
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010

#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b00010
#d8 0b11110
screenBufferEnd:

gameScreen: #str " TTL  \x00\x02\x04\x06 Score"
            #str "Snake!\x01\x03\x05\x07"

bitOffsets:
#d8 0b0000011
#d8 0b0000010
#d8 0b0000001
#d8 0b0010101
#d8 0b0010100
#d8 0b0010011
#d8 0b0010010
#d8 0b0010001
#d8 0b0100101
#d8 0b0100100
#d8 0b0100011
#d8 0b0100010
#d8 0b0100001
#d8 0b0110101
#d8 0b0110100
#d8 0b0110011

controlMap:
bin BTN_LEFT
bin DIR_LEFT
bin BTN_UP
bin DIR_UP
bin BTN_RIGHT
bin DIR_RIGHT
bin BTN_DOWN
bin DIR_DOWN
#d8 0x00

snakeBuffer:
#d400 0x00
snakeBufferEnd:
#d8 0
