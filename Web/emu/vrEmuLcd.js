/*
 * Troy's HD44780U Lcd Display Emulator
 *
 * Copyright (c) 2020 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/VrEmuLcd
 *
 * vrEmuLcdWasm.js must be loaded AFTER this file
 */

vrEmuLcd = {
  newLCD: function() { alert("vrEmuLcdModule not yet loaded"); },
  setLoadedCallback: function(onLoadedCallback) {
    vrEmuLcd._onLoaded = onLoadedCallback;
    if (vrEmuLcd._isLoaded) { onLoadedCallback(); }
  },
  _isLoaded: false,
  _onLoaded: function () {},
  CharacterRom: { A00: 0, A02: 1, European: 1, Japanese: 0 }
};

const LCD_CMD_CLEAR = 0b00000001;
const LCD_CMD_HOME = 0b00000010;
  
const LCD_CMD_ENTRY_MODE = 0b00000100;
const LCD_CMD_ENTRY_MODE_INCREMENT = 0b00000010;
const LCD_CMD_ENTRY_MODE_DECREMENT = 0b00000000;
const LCD_CMD_ENTRY_MODE_SHIFT = 0b00000001;
 
const LCD_CMD_DISPLAY = 0b00001000;
const LCD_CMD_DISPLAY_ON = 0b00000100;
const LCD_CMD_DISPLAY_CURSOR = 0b00000010;
const LCD_CMD_DISPLAY_CURSOR_BLINK = 0b00000001;
  
const LCD_CMD_SHIFT = 0b00010000;
const LCD_CMD_SHIFT_CURSOR = 0b00000000;
const LCD_CMD_SHIFT_DISPLAY = 0b00001000;
const LCD_CMD_SHIFT_LEFT = 0b00000000;
const LCD_CMD_SHIFT_RIGHT = 0b00000100;
  
const LCD_CMD_SET_CGRAM_ADDR = 0b01000000;
const LCD_CMD_SET_DRAM_ADDR = 0b10000000;

vrEmuLcd.Schemes = {
  BlueWhite: {
    BackColor: "#1f1fffff",
    PixelOnColor: "#f0f0ffff",
    PixelOffColor: "#0000e0ff",
    PixelTransColor: "#ff00e0ff"
  },
  GreenBlack: {
    BackColor: "#7DBE00",
    PixelOnColor: "#061E00",
    PixelOffColor: "#6FB900"
  }
}

vrEmuLcdModule = {
  onRuntimeInitialized: function ()
  {
    var _newLcd = vrEmuLcdModule.cwrap('vrEmuLcdNew', 'number', ['number','number','number']);

    var _destroyLCD = vrEmuLcdModule.cwrap('vrEmuLcdDestroy', null, ['number']);
    var _sendCommand = vrEmuLcdModule.cwrap('vrEmuLcdSendCommand', null, ['number','number']);
    var _writeByte = vrEmuLcdModule.cwrap('vrEmuLcdWriteByte', null, ['number','number']);
    var _writeString = vrEmuLcdModule.cwrap('vrEmuLcdWriteString', null, ['number','string']);
    var _getDataOffset = vrEmuLcdModule.cwrap('vrEmuLcdGetDataOffset', 'number', ['number','number','number']);
    var _readByte = vrEmuLcdModule.cwrap('vrEmuLcdReadByte', 'number', ['number']);
    var _updatePixels = vrEmuLcdModule.cwrap('vrEmuLcdUpdatePixels', null, ['number']);
    var _numPixelsX = vrEmuLcdModule.cwrap('vrEmuLcdNumPixelsX', 'number', ['number']);
    var _numPixelsY = vrEmuLcdModule.cwrap('vrEmuLcdNumPixelsY', 'number', ['number']);
    var _pixelState = vrEmuLcdModule.cwrap('vrEmuLcdPixelState', 'number', ['number','number','number']);

    vrEmuLcd.newLCD = function(widthChars, heightChars, characterRom) {
      if (characterRom == null)
      {
        characterRom = vrEmuLcd.CharacterRom.European;
      }
      var lcd = _newLcd(widthChars, heightChars, characterRom);

      return _registerLcd(lcd);
    }

    vrEmuLcd.registerLcd = function(lcd)
    {
      return {
        destroy: function() { _destroyLCD(lcd); },
        sendCommand: function(commandByte) { _sendCommand(lcd, commandByte); },
        writeByte: function(dataByte) { _writeByte(lcd, dataByte); },
        writeString: function(str) { _writeString(lcd, str); },
        getDataOffset: function(screenX, screenY) { _getDataOffset(lcd, screenX, screenY); },
        readByte: function() { return _readByte(lcd); },        
        numPixelsX: _numPixelsX(lcd),
        numPixelsY: _numPixelsY(lcd),
        updatePixels: function() { _updatePixels(lcd); },
        pixelState: function(pixelX, pixelY) { return _pixelState(lcd, pixelX, pixelY); },
        colorScheme: vrEmuLcd.Schemes.BlueWhite,
        render: function(ctx, xPos, yPos, width, height) {

          var xbuffer = 7;
          var ybuffer = 3.5;
          var totalPixelsX = this.numPixelsX + (xbuffer * 2);
          var totalPixelsY = this.numPixelsY + (ybuffer * 2);
          var lcdScale = Math.min(height /totalPixelsY, width / totalPixelsX);

          ctx.fillStyle = this.colorScheme.BackColor;
          ctx.fillRect(xPos, yPos, totalPixelsX * lcdScale, totalPixelsY * lcdScale);
  
          this.updatePixels();
          for (var y = 0; y < this.numPixelsY; ++y) {

            for (var x = 0; x < this.numPixelsX; ++x) {
              var s = this.pixelState(x, y);
              ctx.globalAlpha = 1.0;
              if (s == -1)
              {
                continue;
              }
              
              if (this.colorScheme.PixelTransColor && s > 1)
              {
                ctx.fillStyle = this.colorScheme.PixelTransColor;
                ctx.fillRect(xPos + (x + xbuffer) * lcdScale, yPos + (y + ybuffer) * lcdScale, lcdScale * 0.75, lcdScale * 0.75);
                ctx.globalAlpha = 1.0 - (s / 20.0);
              }

              switch (s & 0x01) {
 
                case 0:
                  ctx.fillStyle = this.colorScheme.PixelOffColor;
                  break;
  
                case 1:
                  ctx.fillStyle = this.colorScheme.PixelOnColor;
                  break;
  
              }
              ctx.fillRect(xPos + (x + xbuffer) * lcdScale, yPos + (y + ybuffer) * lcdScale, lcdScale * 0.75, lcdScale * 0.75);
            }
          }
          ctx.globalAlpha = 1.0;
        }
      }
    }

    vrEmuLcd.isLoaded = true;
    vrEmuLcd._onLoaded();
   }
}