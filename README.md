# vrCPU
Code, documentation, schematics, notes for my [Ben Eater](https://eater.net/8bit "Ben Eater") and [James Bates](https://github.com/jamesbates/jcpu "James Bates") inspired breadboard computer and assembler and web-based [emulator](https://cpu.visualrealmsoftware.com/emu/ "emulator") using a C backend compiled to WASM.

![The real deal](https://cpu.visualrealmsoftware.com/img/computer_gh.jpg "The real deal")

## Structure
### Arduino
* Microcode EEPROM writer
* DecimalDisplay EEPROM writer
* ESP8266 Wi-Fi Program Loader
* Page-write-enabled EEPROM writer library (Tested on Greenliant GLS29EE010)
### Emulator (C library)
* SimLib - The emulator core
* SimInst - A single instance interface of the emulator core
* SimWin - A windows executable around the library (used for testing)
* SimWasm - Emscripten source and scripts to produce WASM output
### Notes
Various files used while building the breadboard computer
### Programs
Various ASM programs used to test the assembler (and the emualtor and physcial computer)
### Web
A copy of most files from [https://cpu.visualrealmsoftware.com](https://cpu.visualrealmsoftware.com "https://cpu.visualrealmsoftware.com") including:
* Assembler derived from [customasm](https://github.com/hlorenzi/customasm "customasm")
* Emulator

## Videos
##### Troy's breadboard computer - Triangular numbers
[![Troy's breadboard computer - Triangular numbers](http://img.youtube.com/vi/Zj5HfeiyHRU/0.jpg)](http://www.youtube.com/watch?v=Zj5HfeiyHRU "My Ben Eater (and James Bates) inspired 8-bit computer..(Triangular numbers)")

##### Troy's breadboard computer - Emulator
[![Troy's breadboard computer - Emulator](http://img.youtube.com/vi/omVyW-ZOdC8/0.jpg)](http://www.youtube.com/watch?v=omVyW-ZOdC8 "Web-based Emulator of my Ben Eater inspired 8-bit computer")

## License
This code is licensed under the [MIT](https://opensource.org/licenses/MIT "MIT") license
