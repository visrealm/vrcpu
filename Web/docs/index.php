<!DOCTYPE html>
<html>
<head>
<title>Troy's Breadboard Computer - Documentation</title>
<link rel="shortcut icon" href="https://visualrealmsoftware.com/sites/default/files/favicon2.png" type="image/png" />
<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
<style>
body {
	background-image: url("../img/computer1.jpg");
    background-repeat:no-repeat;
    background-size:cover;
    background-attachment: fixed;
} 

.code
{
    font-family: monospace;
    font-size: 16pt;
    color: #00c000;
}

h1
{
    margin-top: 40px;
}

h2
{
    margin-top: 40px;
}

h3
{
    font-family: monospace;
    margin-top: 20px;
}


p
{
    padding-left: 20px;
}

</style>


</head>
<script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
<body>
    
<nav class="navbar navbar-expand-lg navbar-dark bg-dark">
  <a class="navbar-brand" href="https://cpu.visualrealmsoftware.com">Troy's Breadboard Computer</a>
  <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNav" aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
    <span class="navbar-toggler-icon"></span>
  </button>
  <div class="collapse navbar-collapse" id="navbarNav">
    <ul class="navbar-nav">
      <li class="nav-item ">
        <a class="nav-link" href="https://cpu.visualrealmsoftware.com/asm">Assembler</a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="https://cpu.visualrealmsoftware.com/emu">Emulator</a>
      </li>
      <li class="nav-item active">
        <a class="nav-link" href="https://cpu.visualrealmsoftware.com/docs">Documentation</a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="https://cpu.visualrealmsoftware.com/gallery">Gallery</a>
      </li>
    </ul>
  </div>
</nav>

<div class="container2 text-light" style="background: #262626f0;
    width: 100%;
    height: 100%;
    padding: 20px;">

<div class="container text-light">

    <h1>REGISTERS: [Rx]</h1>
    <p>This is the list of register mnemonics which can be used in place of [Rx]. All registers are 8-bits.</p>
    <ul>
        <li><span class="code">Ra</span> General purpose register "A"</li>
        <li><span class="code">Rb</span> General purpose register "B" - this register is also the second operand for aritmetic and binary logic operations.</li>
        <li><span class="code">Rc</span> General purpose register "C"</li>
        <li><span class="code">Rd</span> General purpose register "D" - this register is connected to a decimal display.</li>
        <li><span class="code">PC</span> Program counter</li>
        <li><span class="code">SP</span> Stack pointer</li>
        <li><span class="code">Acc</span> Accumulator - this is the output from the ALU</li>
    </ul>

    <h1>ADDRESS MODES:</h1>
    <p>Most instructions take one or two parameters. These can be a register, an address or an immediate value, depending on the instruction.</p>
    <ul>
        <li><span class="code">Rx</span> Any register. eg.<br/><span class="code">mva Rb</span> will copy the contents of Rb to Ra.</li>
        <li><span class="code">#nnn</span> An immediate value. eg.<br/><span class="code">mva #123</span> will write the value 123 to Ra.</li>
        <li><span class="code">0xnn</span> An immediate address. eg.<br/><span class="code">jmp 0x80</span> will jump to address 0x80 (128).</li>
    </ul>

    <h1>CPU FLAGS:</h1>
    <p>Any operation involving the ALU will update the CPU flags as follows:</p>
    <ul>
        <li><span class="code">C</span> Carry: Set if the last operation would cause a carry bit to be passed to the next byte. Can be used for multi-byte arithmetic.</li>
        <li><span class="code">Z</span> Zero: Set if the last operation resulted in a value of 0.</li>
        <li><span class="code">N</span> Negative: Set if the last operation resulted in a negative result.</li>
        <li><span class="code">O</span> Overflow: Set if the last operation resulted in an overflow condition.</li>
    </ul>

    <h1>LABELS:</h1>
    <p>Labels start with "." (eg. .loop). Labels can be referenced anywhere a {value} or {address} is expected.</p>
    <p class="code">
        <strong>.loop:</strong><br/>
        &nbsp;&nbsp;inc Rd<br/>
        &nbsp;&nbsp;jnz <strong>.loop</strong><br/>
    </p>
    <h3>#addr {addr}</h3>
    <p>Allows you to set the address of the current line. eg.<br/>
    <span class="code">#addr 0x80</span><br/>
    <span class="code">.myfunc</span><br/>
    <span class="code">&nbsp;&nbsp;ret</span><br/>
    Will result in <span class="code">.myfunc</span> being placed in memory at address 0x80 (128).
    </p>
    
    <h1>CONSTANTS:</h1>
    <p>Define a constant with the syntax NAME = value. Constants can be used in place of values or addresses. eg.</p>
    <p class="code">
        <strong>MYVALUE = 3</strong><br/>
        &nbsp;&nbsp;data Rd, MYVALUE<br/>
    </p>
    

    <h1>INSTRUCTIONS:</h1>
    <h2>Arithmetic operations</h2>
    <h3>add [Rx]</h3>
    <p>Add Rb to the register [Rx]. Output the result back to [Rx]</p>
    <h3>addc [Rx]</h3>
    <p>Add Rb to the register [Rx], inputting the carry flag if set. Output the result back to [Rx]</p>
    <h3>sub [Rx]</h3>
    <p>Subtract Rb from the register [Rx]. Output the result back to [Rx]</p>
    <h3>subc [Rx]</h3>
    <p>Add Rb to the register [Rx], inputting the carry flag if set. Output the result back to [Rx]</p>
    <h3>sub Rb, [Rx]</h3>
    <p>Subtract [register Rx] from Rb. Output the result back to [Rx]</p>
    <h3>subc Rb, [Rx]</h3>
    <p>Subtract register [Rx] from Rb, inputting the carry flag if set. Output the result back to [Rx]</p>
    <h3>inc [Rx]</h3>
    <p>Increment the value of register [Rx]</p>
    <h3>dec [Rx]</h3>
    <p>Decrement the value of register [Rx]</p>

    <h2>Logic operations</h2>
    <h3>and [Rx]</h3>
    <p>Logical AND or register [Rx] and Rb. Output the result back to [Rx]</p>
    <h3>or [Rx]</h3>
    <p>Logical OR or register [Rx] and Rb. Output the result back to [Rx]</p>
    <h3>xor [Rx]</h3>
    <p>Logical EXCLUSIVE-OR or register [Rx] and Rb. Output the result back to [Rx]</p>
    <h3>not [Rx]</h3>
    <p>Bitwise-invert register [Rx]. Output the result back to [Rx]</p>
    <h3>cmp [Rx]</h3>
    <p>Compare register [Rx] with Rb. Result stored in CPU flags (N if [Rx] less than Rb, Z if [Rx] == Rb)</p>
    <h3>cmp Rb, [Rx]</h3>
    <p>Compare Rb with register [Rx]. Result stored in CPU flags (N if Rb less than [Rx], Z if Rb == [Rx])</p>
    <h3>tst [Rx]</h3>
    <p>Test value stored in register [Rx]. Result stored in CPU flags (N if [Rx] is less than 0, Z if Rb == [Rx])</p>
    <h3>lsr</h3>
    <p>Binary left-shift value stored in Rb.</p>

    <h2>Flow control</h2>
    <h3>jmp {addr}</h3>
    <p>Unconditional jump to the address given in {addr}. Updates the program counter for this address to be executed next.</p>
    <h3>jmp [Rx]</h3>
    <p>Unconditional jump to the address stored in register [Rx]. Updates the program counter for this address to be executed next.</p>
    <h3>jmpz</h3>
    <p>Unconditional jump address 0x00.</p>
    <h3>jc {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU carry (C) flag is set. Otherwise, continue.</p>
    <h3>jn {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU negative (N) flag is set. Otherwise, continue.</p>
    <h3>jo {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU overflow (O) flag is set. Otherwise, continue.</p>
    <h3>jz {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU zero (Z) flag is NOT set. Otherwise, continue.</p>
    <h3>jnc {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU carry (C) flag is NOT set. Otherwise, continue.</p>
    <h3>jnn {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU negative (N) flag is NOT set. Otherwise, continue.</p>
    <h3>jno {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU overflow (O) flag is NOT set. Otherwise, continue.</p>
    <h3>jnz {addr}</h3>
    <p>Jump to the address given in {addr} if the CPU zero (Z) flag is NOT set. Otherwise, continue.</p>

    <h2>Registers</h2>
    <h3>mov [dRx], [sRx]</h3>
    <p>Copy the value stored in register [sRx] to the register [dRx].</p>
    <h3>mov [dRx], #{value}</h3>
    <p>Write immediate {value} to the register [dRx].</p>
    <h3>mv(a|b|c|d) [sRx]</h3>
    <p>Copy the value stored in register [sRx] to the register (a, b, c or d). eg. <span class="code">mva Rb</span> (Copy contents of Rb to Ra)</p>
    <h3>mv(a|b|c|d) #{value}</h3>
    <p>Write immediate {value} to the register (a, b, c or d).</p>
    <h3>data [dRx], #{value}</h3>
    <p>Write immediate {value} to the register [dRx].</p>
    <h3>clr [Rx]</h3>
    <p>Set the value stored in register [Rx] to zero.</p>
    <h3>clra</h3>
    <p>Clear all registers (Ra -> Rd)</p>

    <h2>Memory</h2>
    <h3>lod [dRx], [sRx]</h3>
    <p>Load the value from memory address stored in register [sRx] into register [dRx].</p>
    <h3>lod [dRx], #{value}</h3>
    <p>Load the value from memory address {value} into register [dRx].</p>
    <h3>sto [dRx], [sRx]</h3>
    <p>Store the value from register [sRx] into memory address stored in register [dRx].</p>
    <h3>sto [dRx], #{value}</h3>
    <p>Store the value from register [dRx] into memory address {value}.</p>

    <h2>Stack</h2>
    <h3>push [Rx]</h3>
    <p>Push the value stored in register [Rx] on to the stack.</p>
    <h3>push #{value}</h3>
    <p>Push the {value} on to the stack.</p>
    <h3>pop [Rx]</h3>
    <p>Pop the last value from the stack and place it in register [Rx].</p>
    <h3>pop {addr}</h3>
    <p>Pop the last value from the stack and place it in memory location {addr}.</p>
    <h3>peek [Rx]</h3>
    <p>Place the last value from the stack in register [Rx], however don't update the stack pointer.</p>
    <h3>peek {addr}</h3>
    <p>Place the last value from the stack in memory at location {addr}, however don't update the stack pointer.</p>
    <h3>call</h3>
    <p>Call the function at the address stored in register Rc. Pushes the current address to the stack and sets the program counter to Rc.</p>
    <h3>call {addr}</h3>
    <p>Call the function at the address {addr}. Pushes the current address to the stack and sets the program counter to {addr}. <strong>Note:</strong> this instruction replaces the contents of Rc.</p>
    <h3>ret</h3>
    <p>Return to the address after the last function call. Pops the address from the stack and sets the program counter.</p>
    
    <h2>Miscellaneous</h2>
    <h3>nop</h3>
    <p>No-operation. Do nothing for one instruction.</p>
    <h3>hlt</h3>
    <p> Halt. Stops the clock.</p>
    
    </div>
</div>

</body>
</html>