#cpudef
{
    #bits 8
    
    #tokendef reg
    {
        Ra  = 0b000
        Rb  = 0b001
        Rc  = 0b010
        Rd  = 0b011
        SP  = 0b100
        PC  = 0b101
        SPi = 0b110
        Acc = 0b110
        Imm = 0b111
    }

                
    nop                          -> 0b00 @ 0b000 @ 0b000
    hlt                          -> 0b00 @ 0b101 @ 0b101
    mov  {dest: reg}, {src: reg} -> 0b00 @ dest[2:0] @ src[2:0]
    mov  {dest: reg}, #{value}    -> 0b00 @ dest[2:0] @ 0b111 @ value[7:0]
    mva  {src: reg}              -> 0b00 @ 0b000 @ src[2:0]
    mvb  {src: reg}              -> 0b00 @ 0b001 @ src[2:0]
    mvc  {src: reg}              -> 0b00 @ 0b010 @ src[2:0]
    mvd  {src: reg}              -> 0b00 @ 0b011 @ src[2:0]
    acc  {src: reg}              -> 0b00 @ 0b110 @ src[2:0]
    mva  #{value}                -> 0b00 @ 0b000 @ 0b111 @ value[7:0]
    mvb  #{value}                -> 0b00 @ 0b001 @ 0b111 @ value[7:0]
    mvc  #{value}                -> 0b00 @ 0b010 @ 0b111 @ value[7:0]
    mvd  #{value}                -> 0b00 @ 0b011 @ 0b111 @ value[7:0]
    acc  #{value}                -> 0b00 @ 0b110 @ 0b111 @ value[7:0]
                
    data {dest: reg}, #{value}   -> 0b00 @ dest[2:0] @ 0b111 @ value[7:0]
    data {dest: reg}, {addr}     -> 0b00 @ dest[2:0] @ 0b111 @ addr[7:0]
    jmp  {addr}                  -> 0b00 @ 0b101 @ 0b111 @ addr[7:0]
    jmp  {src: reg}              -> 0b00 @ 0b101 @ src[2:0]
    jmz                          -> 0b00 @ 0b110 @ 0b101
            
    jc   {addr}                  -> 0b00 @ 0b111 @ 0b000 @ addr[7:0]
    jz   {addr}                  -> 0b00 @ 0b111 @ 0b001 @ addr[7:0]
    jo   {addr}                  -> 0b00 @ 0b111 @ 0b010 @ addr[7:0]
    jn   {addr}                  -> 0b00 @ 0b111 @ 0b100 @ addr[7:0]
                
    jnc  {addr}                  -> 0b00 @ 0b111 @ 0b111 @ addr[7:0]
    jnz  {addr}                  -> 0b00 @ 0b111 @ 0b110 @ addr[7:0]
    jno  {addr}                  -> 0b00 @ 0b111 @ 0b101 @ addr[7:0]
    jnn  {addr}                  -> 0b00 @ 0b111 @ 0b011 @ addr[7:0]
                                
    lod  {dest: reg}, {src: reg} -> 0b01 @ dest[2:0] @ src[2:0]
    lod  {dest: reg}, {value}    -> 0b01 @ dest[2:0] @ 0b111 @ value[7:0]

    sto  {dest: reg}, {src: reg} -> 0b10 @ dest[2:0] @ src[2:0]
    sto  {value}, {src: reg}     -> 0b10 @ 0b111 @ src[2:0] @ value[7:0]

    push {src: reg}              -> 0b10 @ 0b110 @ src[2:0]
    push #{value}                -> 0b10 @ 0b110 @ 0b111 @ value[7:0]
    pop  {dest: reg}             -> 0b01 @ dest[2:0] @ 0b110
    pop  {addr}                  -> 0b01 @ 0b111 @ 0b110 @ addr[7:0]
    peek{dest: reg}             -> 0b01 @ 0b110 @ dest[2:0]
    peek {addr}                  -> 0b01 @ 0b110 @ 0b111 @ addr[7:0]
  
    call                         -> 0b10 @ 0b110 @ 0b101 ; Call address in #Rc
    call {addr}                  -> 0b10 @ 0b111 @ 0b101 @ addr[7:0]; Call address (trashes #Rc)
    ret                          -> 0b01 @ 0b101 @ 0b110

    clr  {dest: reg}             -> 0b00 @ 0b110 @ dest[2:0]
    clra                         -> 0b00 @ 0b110 @ 0b111

    inc  {dest: reg}             -> 0b110 @ 0b000 @ dest[1:0]
    dec  {dest: reg}             -> 0b111 @ 0b000 @ dest[1:0]
                
    add  {dest: reg}, Rb         -> 0b110 @ 0b011 @ dest[1:0]
    addc  {dest: reg}, Rb         -> 0b111 @ 0b011 @ dest[1:0]
    sub  {dest: reg}, Rb         -> 0b110 @ 0b010 @ dest[1:0]
    subc {dest: reg}, Rb         -> 0b111 @ 0b010 @ dest[1:0]
    add  {dest: reg}             -> 0b110 @ 0b011 @ dest[1:0]
    addc {dest: reg}             -> 0b111 @ 0b011 @ dest[1:0]
    sub  {dest: reg}             -> 0b110 @ 0b010 @ dest[1:0]
    subc {dest: reg}             -> 0b111 @ 0b010 @ dest[1:0]
    sub  Rb, {src: reg}          -> 0b110 @ 0b001 @ src[1:0]
    subc Rb, {src: reg}          -> 0b111 @ 0b001 @ src[1:0]
    lsr                          -> 0b110 @ 0b011 @ 0b01

    and  {dest: reg}, Rb         -> 0b110 @ 0b110 @ dest[1:0]
    or   {dest: reg}, Rb         -> 0b110 @ 0b101 @ dest[1:0]
    xor  {dest: reg}, Rb         -> 0b110 @ 0b100 @ dest[1:0]
    and  {dest: reg}             -> 0b110 @ 0b110 @ dest[1:0]
    or   {dest: reg}             -> 0b110 @ 0b101 @ dest[1:0]
    xor  {dest: reg}             -> 0b110 @ 0b100 @ dest[1:0]
    not  {dest: reg}             -> 0b110 @ 0b111 @ dest[1:0]
    cmp  Rb, {dest: reg}         -> 0b111 @ 0b101 @ dest[1:0]
    cmp  {dest: reg}, Rb         -> 0b111 @ 0b100 @ dest[1:0]
    cmp  {dest: reg}             -> 0b111 @ 0b100 @ dest[1:0]
    tst  {src: reg}              -> 0b00 @ 0b110 @ src[2:0]

    lcc #{imm}                   -> 0b01 @ 0b111 @ 0b110 @ imm[7:0]
    lcd #{imm}                   -> 0b01 @ 0b111 @ 0b111 @ imm[7:0]
    lcc  {src: reg}              -> 0b111 @ 0b100 @ src[1:0]
    lcd  {src: reg}              -> 0b111 @ 0b111 @ src[1:0]

}

#addr 0x000
