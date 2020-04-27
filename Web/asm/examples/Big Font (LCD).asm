; ===============================================
; Big Font Demo
; -----------------------------------------------
; This demo uses the character LCD
; It generates a set of custom characters which
; are combined to form a custom double-row font
; The font data is stored in the data region of
; memory.
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
SCROLL_LEFT = LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_LEFT
SCROLL_RIGHT = LCD_CMD_SHIFT | LCD_CMD_SHIFT_DISPLAY | LCD_CMD_SHIFT_RIGHT
NEXTLINE = LCD_CMD_SET_DRAM_ADDR | 0x40

CGRAM_START = LCD_CMD_SET_CGRAM_ADDR
CGRAM_END = CGRAM_START + 8 * 8 - 1
AUTO_REVERSE = LCD_CMD_ENTRY_MODE | LCD_CMD_ENTRY_MODE_DECREMENT
AUTO_FORWARD = LCD_CMD_ENTRY_MODE | LCD_CMD_ENTRY_MODE_INCREMENT

; where on the character LCD to generate characters
; when this is greater than the width of the display
; characters will be formed before being scrolled in
; to view
XOFFSET = 16 

; maximum X location supported by the LCD.
; this is used so we know to move back to 0 while 
; outputting the character data
MAX_X   = 40

; gap (in LCD character columns) between each 
; display of the text
LOOP_GAP = 16

; ascii space characer
ASCII_SPACE = 32

; data offset in RAM. if this is 0, then the pointer
; to space (' ') is stored in RAM at offset 32, however
; that reduces the space available for character data
; so I offset it.  this must be less than ASCII_SPACE
DATA_OFFSET = -27

; where in the program do we start the character data
CHAR_DATA_START = RAM_OFFSET + ASCII_SPACE + DATA_OFFSET

; RAM locations for variables
_CURRENT_SCREEN_OFFSET = 0x00
_STRING_PTR            = 0x01
_CHAR_DATA_PTR         = 0x02
_RESETTING             = 0x03

; jump the character data
jmp start

; the nulll-delimited text to display.
text: #str "HTTPS:/CPU.VISUALREALMSOFTWARE.COM \0"


; start of the program
start:
	lcc #LCD_INITIALIZE
	lcc #DISPLAY_MODE
	lcc #LCD_CMD_CLEAR

	clra
	push Ra
	call buildCustomCharacters
	data Rb, XOFFSET
	sto Rb, _CURRENT_SCREEN_OFFSET	
	jmp .afterLoopGap
	
.resetStringPointers:
	pop Rb
	data Ra, LOOP_GAP
	push Ra
.afterLoopGap:
	data Rb, text
	sto Rb, _STRING_PTR	
	clr Rb
	sto Rb, _CHAR_DATA_PTR
	
.loop:
	pop Ra
	tst Ra
	jz .testStringEnd
	dec Ra
	
.testStringEnd:
	push Ra
	lod Rc, _STRING_PTR
	lod Rb, Rc
	tst Rb
	jz .resetStringPointers

	lod Rb, _CURRENT_SCREEN_OFFSET
	inc Rb
	data Rc, MAX_X
	cmp Rb, Rc
	jnz .offsetFixed
	sub Rc
	mov Rb, Rc
	
.offsetFixed:
	sto Rb, _CURRENT_SCREEN_OFFSET
	data Ra, LCD_CMD_SET_DRAM_ADDR
	add Ra
	lcc Ra
		
	data Rb, 0xee
	peek Ra
	tst Ra
	jnz .outputTop
	lod Ra, _CHAR_DATA_PTR
	tst Ra
	jz .charDataNotOk
	lod Rb, Ra
	tst Rb
	jnz .charDataPtrOk
	.charDataNotOk:
	lod Rc, _STRING_PTR
	
	data Ra, DATA_OFFSET
	lod Rb, Rc
	add Ra
	mov Rb, Ra
	
	inc Rc
	sto Rc, _STRING_PTR
	lod Ra, Rb    ; get character pointer
	sto Ra, _CHAR_DATA_PTR
	data Rb, 0xee
	jmp .outputTop

.charDataPtrOk:
	lod Rb, Ra
	inc Ra
	sto Ra, _CHAR_DATA_PTR
.outputTop:
	
	call getChars
	lcd Rc
	push Ra
	
	lod Ra, _CURRENT_SCREEN_OFFSET
	data Rb, NEXTLINE
	add Ra
	lcc Ra
		
	pop Rc
	lcd Rc
	
	data Rb, 100
	call delay
	
	data Ra, SCROLL_LEFT
	lcc Ra
	
	jmp .loop

delay:
	dec Rb
	jnz delay
	ret

; leftRotate()
; bitwise rotate Rb
leftRotate:
	lsr
	jnc .done
	inc Rb
.done:
	ret
; end leftRotate()


; translate a character definition byte into the 
; two character codes required for a column of
; the LCD display
;
; inputs:
;   Rb = byte (high nibble = top, low nibble = bottom)
; returns:
;   Rc = top character code
;   Ra = bottom character code
getChars:
	push Rb
	data Ra, 0x0f
	and Ra
	call getChar
	pop Rb
	push Rc
	data Rc, leftRotate
	call
	call 
	call 
	call 
	data Ra, 0x0f
	and Ra
	call getChar
	pop Ra
	ret
; end getChars()

; Map a character nibble to the corresponding
; LCD character code
;
; inputs:
;   Ra = nibble
; returns:
;   Rc = LCD character code
getChar:
	tst Ra
	jz .space
	data Rb, 0x0e
	cmp Ra, Rb
	jn .done
	jz .space
	data Rc, 0xff
	ret	
.space:
	data Rc, ASCII_SPACE
	ret
.done:
	mov Rc, Ra
	ret
; end getChar()


; write the custom characters to the LCD CGRAM
buildCustomCharacters:	
	data Ra, CGRAM_START
	lcc Ra
	data Rb, (charDataEnd - charData)
	data Rc, charData
.addLine:
	lod Ra, Rc
	lcd Ra
	inc Rc
	dec Rb
	jnz .addLine
	data Rb, (charDataEnd - charData)
	data Rc, charDataEnd - 1
.addRLine:
	lod Ra, Rc
	lcd Ra
	dec Rc
	dec Rb
	jnz .addRLine
	ret
; end buildCustomCharacters()


; ----------------------------
; custom character definitions
; ----------------------------
; these are mapped at LCD code 0 - 3. 
; then vertically flipped at 7 - 4. 
; 0 is referenced using 8 to avoid
; confusion with a null terminator
charData: 
; 08 (07)
#d8 0b00001   
#d8 0b00011
#d8 0b00011
#d8 0b00111
#d8 0b00111
#d8 0b01111
#d8 0b01111
#d8 0b01111

; 01 (06)
#d8 0b11111
#d8 0b11111
#d8 0b11111
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b11111
#d8 0b11111

; 02 (05)
#d8 0b10000
#d8 0b11000
#d8 0b11000
#d8 0b11100
#d8 0b11100
#d8 0b11110
#d8 0b11110
#d8 0b11110

; 03 (04)
#d8 0b11111
#d8 0b11111
#d8 0b11111
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
#d8 0b00000
charDataEnd:
; end of program memory space

; ===========================
; start of RAM
; ===========================
#addr CHAR_DATA_START

; ---------------------------
; character map
; ---------------------------
; the ascii character maps to this location using
; DATA_OFFSET (eg. char + DATA_OFFSET)
; then that location contains a pointer to the character
; data. unmapped characters map to NULL
bin SPACE         ; ' '
bin EXCLAMATION   ; !
bin DOUBLE_QUOTE  ; "
bin NULL          ; #
bin NULL          ; $
bin NULL          ; %
bin NULL          ; &
bin SINGLE_QUOTE  ; '
bin OPEN_BRACKET  ; (
bin CLOSE_BRACKET ; )
bin NULL          ; *
bin PLUS          ; +
bin NULL          ; ,
bin MINUS         ; -
bin STOP          ; .
bin SLASH         ; /
bin ZERO          ; 0
bin ONE           ; 1
bin TWO           ; 2
bin THREE         ; 3
bin FOUR          ; 4
bin FIVE          ; 5
bin SIX           ; 6
bin SEVEN         ; 7
bin EIGHT         ; 8
bin NINE          ; 9
bin COLON         ; :
bin NULL          ; ;
bin LT            ; <
bin EQ            ; =
bin GT            ; >
bin QUES          ; ?
bin NULL          ; @
bin A             ; A
bin B             ; B
bin C             ; C
bin D             ; D
bin E             ; E
bin F             ; F
bin G             ; G
bin H             ; H
bin I             ; I
bin J             ; J
bin K             ; K
bin L             ; L
bin M             ; M
bin N             ; N
bin O             ; O
bin P             ; P
bin Q             ; Q
bin R             ; R
bin S             ; S
bin T             ; T
bin U             ; U
bin V             ; V
bin W             ; W
bin X             ; X
bin Y             ; Y
bin Z             ; Z

; ---------------------------
; character data
; ---------------------------
; unused characters can be commented-out to save memory.
; each characher consists of one byte per column.
; each nibble maps to an LCD character:
;
;   1-8: maps to the custom characters defined at charData
;   e  : space
;   f  : full block
;
; every character definition must be null-terminated 

NULL:          #d8 0x00
SPACE:         #d16 0xee00
EXCLAMATION:   #d16 0xf600
DOUBLE_QUOTE:  ;#d32 0x3eee3e00
SINGLE_QUOTE:  #d16 0x3e00
OPEN_BRACKET:  #d16 0x4e00;24 0x873400
CLOSE_BRACKET: #d16 0xe300;24 0x342500
PLUS:          #d32 0x43ff4300

MINUS:         #d32 0x43434300
STOP:          #d16 0xe400
SLASH:         ;#d32 0xe8855e00
SLASHES:       #d48 0xe88558855e00
ZERO:          #d32 0x87342500
ONE:           #d32 0x34ffe400
TWO:           #d32 0x17142400
THREE:         #d32 0x14142500
FOUR:          #d32 0x7e4eff00
FIVE:          #d32 0xf4141500
SIX:           #d32 0x87141500
SEVEN:         #d32 0x3e385e00
EIGHT:         #d32 0x87142500
NINE:          #d32 0x84142500
COLON:         #d16 0x4400
LT:            ;#d24 0x875200
EQ:            #d24 0x444400
GT:            ;#d24 0x782500
QUES:          #d32 0x3e162e00
A:             #d32 0x8f1e2f00
B:             #d32 0xff142500
C:             #d32 0x87343400
D:             #d32 0xff342500
E:             #d32 0xff141400
F:             #d32 0xff1e1e00
G:             #d32 0x8f141f00
H:             #d32 0xff4eff00
I:             #d32 0x34ff3400
J:             #d24 0x34f500
K:             #d32 0xff4e5200
L:             #d24 0xffe400
M:             #d40 0xff2785ff00
N:             #d32 0xff27ff00
O:             #d32 0x87342500
P:             #d32 0xff1e2e00
Q:             #d32 0x87342f00
R:             #d32 0xff1e2200
S:             #d32 0x84141500
T:             #d32 0x3eff3e00
U:             #d32 0xf7e4f500
V:             #d32 0x27e48500
W:             #d40 0xff8527ff00
X:             #d32 0x784e5200
Y:             #d32 0x7e4f5e00
Z:             #d32 0x38145400
