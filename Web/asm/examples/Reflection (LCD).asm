DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON
SCROLL_LEFT = LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_LEFT
SCROLL_RIGHT = LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_RIGHT
NEXTLINE = LCD_CMD_SET_DRAM_ADDR | 40


lcc #LCD_INITIALIZE
lcc #DISPLAY_MODE

start:
	clra
	lcc #LCD_CMD_CLEAR
	call buildCustomCharacters
	lcc #LCD_CMD_SET_DRAM_ADDR
	data Ra, helloStr
	call printStr
	lcc #NEXTLINE
	data Ra, unsideDnStr
	call printStr

	clr Rb
	data Ra, 4
.scrollRight:
	lcc #SCROLL_RIGHT
	inc Rb
	cmp Ra
	jnz .scrollRight

.scrollLeft:
	lcc #SCROLL_LEFT
	dec Rb
	jnz .scrollLeft
	jmp .scrollRight

buildCustomCharacters:	
	lcc #LCD_CMD_SET_CGRAM_ADDR
	data Rb, (charDataEnd - charData)
	data Rc, charData
.addLine:
	lod Ra, Rc
	lcd Ra
	inc Rc
	dec Rb
	jnz .addLine
	ret

printStr:
	mov Rc, Ra
.nextChar:
	lod Ra, Rc
	tst Ra
	jz .done
	lcd Ra
	inc Rc
	jmp .nextChar
.done:
    ret

helloStr: #str "Hello World!\0"
unsideDnStr: #d104 0x08010202032004030502060700
charData: 
#d64 0x001111111f111111 ; H
#d64 0x000e101f110e0000 ; e
#d64 0x000e04040404040c ; l
#d64 0x000e1111110e0000 ; o
#d64 0x000a151515111111 ; W
#d64 0x0010101019160000 ; r
#d64 0x000f1111130d0101 ; d
#d64 0x0004000004040404 ; !
charDataEnd: