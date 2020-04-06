; Demonstration of CGRAM in the character display
; Displays the Commodore computers logo

DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON
NEXTLINE = LCD_CMD_SET_DRAM_ADDR | 40
C3END = LCD_CMD_SET_CGRAM_ADDR | (8*4-1)
FORWARD = LCD_CMD_ENTRY_MODE | LCD_CMD_ENTRY_MODE_INCREMENT

initializeDisplay:
	clra
	lcc #LCD_INITIALIZE
	lcc #DISPLAY_MODE
	lcc #LCD_CMD_CLEAR


	call setCharset1
	
outputCharacters:
	data Rb, 10
	.topRowChars:
	lcd #0
	lcd #32
	lcd #32
	lcd #2
	dec Rb
	jnz .topRowChars

	data Rb, 10
	.botRowChars:
	lcd #32
	lcd #1
	lcd #3
	lcd #32
	dec Rb
	jnz .botRowChars
	
alternate:	
	;call setCharset2
	;call setCharset1
	;jmp alternate
	
	jmp scrollDisplay


buildCharset:
	push Ra
	push Rb
	lcc #LCD_CMD_SET_CGRAM_ADDR
	call buildCustomCharacters
	lcc #LCD_CMD_ENTRY_MODE
	lcc #C3END
	pop Rb
	pop Ra
	call buildCustomCharacters
	lcc #LCD_CMD_SET_DRAM_ADDR
	lcc #FORWARD	
	ret
	
setCharset1:
	data Ra, charDataStart
	data Rb, charDataEnd
	call buildCharset
	ret
	
setCharset2:
	data Ra, charDataStart2
	data Rb, charDataEnd2
	call buildCharset
	ret

	
buildCustomCharacters:
	mov Rc, Ra
	sub Rb, Ra
	mov Rb, Ra
	.addLine:
		lod Ra, Rc
		lcd Ra
		inc Rc
		dec Rb
		jnz .addLine
	ret

SCROLL_RIGHT = LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_RIGHT

scrollDisplay:
	lcc #SCROLL_RIGHT
	jmp scrollDisplay

charDataStart:
#d64 0x1804040202020101
#d64 0x1010080808040403
charDataEnd:
charDataStart2:
#d64 0x0018040402020201
#d64 0x1008080804040300
charDataEnd2:
