lcc #0x38
lcc #0x0f

.start:
  clra
  lcc #1
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
