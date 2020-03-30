DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON | LCD_CMD_DISPLAY_CURSOR | LCD_CMD_DISPLAY_CURSOR_BLINK

lcc #LCD_INITIALIZE
lcc #DISPLAY_MODE

.start:
  clra
  lcc #LCD_CMD_CLEAR
  data Ra, .hello
  call .printStr
  jmz

.printStr:
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

.hello:
#str "Hello, World!\0"