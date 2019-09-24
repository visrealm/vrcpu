; Show all power series (n^1, n^2, n^3, ...) up to base 15. Limited
; to results <= 255
; Adapted from the program by James Bates at https://github.com/jamesbates/jcpu

.begin:
	data Ra, #1
	sto 0x00, Ra
	data Rb, #1
	sto 0x01, Rb
	
.next:
    data Rc, .dopow
	call
	lod Rb, 0x01
	tst Rb
	jnz .next
	jmz
	
.dopow:
	lod Ra, 0x00
	inc Ra
	data Rb, #15
	and Ra
	jz .restart
	sto 0x00, Ra
	mov Rd, Ra
	mov Rb, Ra
	
.loop0:
	data Rc, #0
	
.loop1:
	dec Ra
	jc .cont
	mov Rd, Rc
	mov Rb, Rc
	lod Ra, 0x00
	jmp .loop0
	
.cont:
	add Rc, Rb
	jc .ret
	jmp .loop1
	
.restart:
	data Rb, #0
	sto 0x01, Rb
	
.ret:
	ret