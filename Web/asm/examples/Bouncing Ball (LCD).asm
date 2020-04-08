; ===============================================
; Bounce a ball around the character LCD
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

; display mode flags
DISPLAY_MODE = LCD_CMD_DISPLAY | LCD_CMD_DISPLAY_ON

; memory address constants
; -----------------------------
_PIXELX        = 0x00        ; ball overall X position
_DIR_X         = _PIXELX + 1 ; ball X direction
_PIXELY        = 0x02        ; ball overall Y position
_DIR_Y         = _PIXELY + 1 ; ball Y direction
_CHARX         = 0x04   	 ; character offset within character LCD
_PIXEL_OFFSETX = 0x06        ; pixel X offset within the character
_LAST_CHARX    = 0x08        ; character offset for the last frame
_CHARSET       = 0x0a        ; character set (0 or 4) 

; other constants
;------------------------------
CHAR_WIDTH      = 5 ; character size (px)
CHAR_HEIGHT     = 8

BALL_HEIGHT     = 4 ; ball bitmap size
BALL_WIDTH      = 4

SCREEN_WIDTH    = 80 ; screen size (px)
SCREEN_HEIGHT   = 16

PLAYABLE_WIDTH  = SCREEN_WIDTH - BALL_WIDTH
PLAYABLE_HEIGHT = SCREEN_HEIGHT - BALL_HEIGHT

NEXTLINE = 0x40 | LCD_CMD_SET_DRAM_ADDR

main:
	clra
	
	; initialise the display
	lcc #LCD_INITIALIZE
	lcc #DISPLAY_MODE
	lcc #LCD_CMD_CLEAR
	
	; store some initial values 
	
	; all 0's
	sto Ra, _PIXELY

	; all 1's
	inc Ra
	sto Ra, _PIXELX
	sto Ra, _DIR_Y
	sto Ra, _LAST_CHARX ; ensure last char positions are.. different (1)

	; all 2's
	inc Ra
	sto Ra, _DIR_X

; -----------------------------------------------
; nextFrame
; -----------------------------------------------
; main program loop
; -----------------------------------------------
nextFrame:
	; check X position is within bounds
	data Rb, PLAYABLE_WIDTH - 1
	data Ra, _PIXELX
	call checkBounds

	; check Y position is within bounds
	data Rb, PLAYABLE_HEIGHT
	data Ra, _PIXELY
	call checkBounds
	
	; compute LCD X character location and pixel offset
	data Rb, CHAR_WIDTH
	lod Ra, _PIXELX
	call div8
	sto Rc, _CHARX
	sto Ra, _PIXEL_OFFSETX

	; which character set are we using for the current
	; character offset?
	call determineCharset
	
	; set up CGRAM address for current y offset and
	; character set
	lod Ra, _PIXELY	
	call setCgRamAddr
	
	; output the first column of ball data to the CGRAM
	mov Ra, ballData	
	call updateChar
	
	; set up CGRAM address for current y offset and
	; character set. For the 2nd column, the CGRAM has
	; an addtional offset of 16 rows of character data
	lod Ra, _PIXELY
	data Rb, CHAR_HEIGHT * 2
	add Ra
	call setCgRamAddr

	; output the second column of ball data to the CGRAM
	; column 2 has an offset of 2 byes into the ball data array
	mov Ra, ballData + 2
	call updateChar

	; output the current character set characters to the
	; character LCD if character x position is different 
	call outputCharacters

	; update the ball location by adding the ball direction
	; offsets
	lod Ra, _PIXELX
	lod Rb, _DIR_X
	add Ra
	sto Ra, _PIXELX
	lod Ra, _PIXELY
	lod Rb, _DIR_Y
	add Ra
	sto Ra, _PIXELY
	
	; do it all again
	jmp nextFrame
; end nextFrame


; -----------------------------------------------
; setCgRamAddr
; -----------------------------------------------
; set the cgram address. if it's not 0, then
; go back one more and clear it too. The CGRAM 
; address is computed: CHARSET * 4 + Y offset
;  
; inputs:
; 	Ra = Y offset
; -----------------------------------------------
setCgRamAddr:
	; get the current character set (0 or 4)
	lod Rb, _CHARSET
	
	; multiply by 8 (8 rows per character)
	lsr
	lsr
	lsr

	clr Rc ; Flag to indicate if we are clearing previous row
	
	; determine if we are are clearing the previous
	; row of CGRAM
	add Ra
	jz .setAddr
	
	; we are, so set RC as an indicator flag
	; and decrement the CGRAM pointer (Ra)
	inc Rc
	dec Ra
	
	; set the address
	.setAddr:
		data Rb, LCD_CMD_SET_CGRAM_ADDR
		add Ra
		lcc Ra
		
	; now check our indicator. if set, we alos outpt a 0
	; to clear the previous row
	tst Rc
	jz .ret
	clr Rc
	lcd Rc
	
	.ret:
		ret
; end setCgRamAddr


; -----------------------------------------------
; updateChar
; -----------------------------------------------
; update CGRAM with character data. this function
; writes 5 bytes to the CGRAM:
;  
; inputs:
;   Ra = address of character data at offset 0
; -----------------------------------------------
updateChar:
	; compute the actual data address by adding
	; 4 * PIXEL_OFFSETX
	lod Rb, _PIXEL_OFFSETX
	lsr
	lsr
	add Ra
	
	; Ra now points to the first row of character
	; data
	
	; load the two rows of character data we need
	; first byte into Rb, second into Ra.
	mov Rc, Ra
	lod Rb, Rc
	inc Rc
	lod Ra, Rc
	
	; output the character rows:
	lcd Rb
	lcd Ra
	lcd Ra
	lcd Rb
	lcd #0 ; clear the last row
	ret
; end updateChar


; -----------------------------------------------
; checkBounds
; -----------------------------------------------
; checks an axis is within bounds of the display
; if it's not, then adjust the position within 
; display limits and also reverse the direction
; can be used to check either X or Y
;  
; inputs:
;   Ra = address of position variable
;   Rb = maximum position value
; -----------------------------------------------
checkBounds:
	; push position address for later use
	push Ra
	
	; load and test the position
	lod Ra, Ra
	tst Ra
	
	; if it's negative, set to 0 and reverse direction
	jn .fixNeg
	
	; if it's greater than maximum position, set to maximum
	; position (Rb) and reverse direction
	cmp Rb, Ra
	jnn .done

	.fixPos:
		jmp .reverse
	
	.fixNeg:
		; new position: 0
		clr Rb
	
	.reverse:
		; position will now be set to value in Rb
		pop Ra
		sto Ra, Rb
		
		; direction is at the next address after 
		; position. load it, reverse it, save it
		inc Ra
		lod Rb, Ra
		clr Rc
		sub Rc
		sto Ra, Rc
		inc Rd
		ret
	.done:
		pop Ra
		ret
; end checkBounds

; -----------------------------------------------
; determineCharset
; -----------------------------------------------
; sets CHARSET to 0 if CHARX is even, 4 if it's odd
; -----------------------------------------------
determineCharset:	
	; load character position
	lod Ra, _CHARX

	; get first bit and multiply by 4
	data Rb, 1
	and Ra
	mov Rb, Ra
	lsr
	lsr
	; Rb now contains 0 or 4
	
	; compare to current character set
	lod Ra, _CHARSET
	cmp Ra
	jz .ret
	
	; character set has changed
	; store it and clear the character data
	sto Rb, _CHARSET
	call clearChars
	
	.ret:
		ret
; end determineCharset

; -----------------------------------------------
; outputCharacters
; -----------------------------------------------
; output the required characters to the display
; if the chracter X position hasn't changed since
; the last frame, do nothing, otherwise:
;
;      Charset 0              Charset 4
;   ' '  0   2  ' '        ' '  4   6  ' '
;   ' '  1   3  ' '        ' '  5   7  ' '
;
; -----------------------------------------------
outputCharacters:
	; check if we need to do anything
	lod Rb, _LAST_CHARX
	lod Ra, _CHARX
	cmp Ra
	jz .ret

	; update last position to current position
	sto Ra, _LAST_CHARX

	; if the offset isn't 0, we output a leading
	; space character. adjust the offset accordingly
	clr Rc
	tst Ra
	jz .doOutput
	dec Rc
	
	.doOutput:
		; at this point:
		;   Ra is the x offset
		;   Rc is the optional leading space offset (0 or -1)
		; 
		; set up the dram address command
		data Rb, LCD_CMD_SET_DRAM_ADDR
		add Ra
		mov Rb, Rc
		add Ra
		lcc Ra  ; set the dram location

		; output the first row of characters
		lod Rb, _CHARSET
		call .outputChars

		; set the dram address to the next line
		data Rb, NEXTLINE
		lod Ra, _CHARX
		add Ra
		mov Rb, Rc
		add Ra
		lcc Ra  ; set the dram location
		
		; output the 2nd row of characters
		lod Rb, _CHARSET
		inc Rb
		call .outputChars

	.ret:
		ret
		
	; Rb will be initial character offset
	; Rc will be 0 (no space) or -1 (leading space)
	.outputChars:
		tst Rc
		jz .skipSpace
		lcd #32
		.skipSpace:
		lcd Rb
		inc Rb
		inc Rb
		lcd Rb
		lcd #32
		ret
; end outputCharacters

; -----------------------------------------------
; clearChars
; -----------------------------------------------
; clear the cgram data for the current character 
; set. 
; -----------------------------------------------
clearChars:
	; set the address to the beginning of the 
	; character set
	clr Ra
	call setCgRamAddr
	clr Ra
	
	; clear the data 8 x 4 = 32 bytes
	; loop unrolled for minor performance win
	data Rb, 8
	.next:
	  lcd Rc
	  lcd Rc
	  lcd Rc
	  lcd Rc
	  dec Rb
	  jnz .next
	ret
; end clearChars

; -----------------------------------------------
; div8
; -----------------------------------------------
; divide two 8-bit integers
;
; inputs:
;  Ra = dividend
;  Rb = divisor
;
; returns:
;  Rc = result
;  Ra = remainder
; -----------------------------------------------
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
; end div8
		

; -----------------------------------------------
; The ball data
; -----------------------------------------------
; Each 32-bit row contains the pixel data for the
; ball at a given X offset.
; The lowest significant 5 bits of each byte are 
; used. The pixel data is arranged like this to 
; form an image:
;
;   B0  B2
;   B1  B3
;   B1  B3
;   B0  B2
; 
; so, the ball will be vertically symmetrical
; to switch ball images, comment out the old and
; uncomment the new
; -----------------------------------------------

ballData:

; filled ball
;#d32 0x0c1e0000
;#d32 0x060f0000
;#d32 0x03070010
;#d32 0x01031018
;#d32 0x0001181c

; empty ball
;#d32 0x0c120000
;#d32 0x06090000
;#d32 0x03040010
;#d32 0x01021008
;#d32 0x00011804

; square ball
;#d32 0x1e1e0000
;#d32 0x0f0f0000
;#d32 0x07071010
;#d32 0x03031818
;#d32 0x01011c1c

; empty square
#d32 0x1e120000
#d32 0x0f090000
#d32 0x07041010
#d32 0x03021808
#d32 0x01011c04
