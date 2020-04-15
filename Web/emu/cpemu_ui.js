/*
 * Troy's 8-bit computer - Web front-end for Troy's cpemu WASM library
 * 
 * Copyright (c) 2019 Troy Schrapel
 * 
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrcpu
 *
 */

function getQueryParams()
{
  var query = window.location.search.substring(1);
  if (!query)
  {
    return {};
  }

  return _.chain(query.split('&'))
    .map(function (params)
    {
      var p = params.split('=');
      return [p[0], decodeURIComponent(p[1])];
    }).object()
    .value();
};

function getParam(key)
{
  var params = getQueryParams();
  if (key in params)
    return params[key];
  return "";
};


var canv = document.getElementById('canv'),
  ctx = canv.getContext('2d');
var img = document.getElementById("img_cpu_base");
var lcdimg = document.getElementById("img_lcd1602");
var glow_red = document.getElementById("img_red_glow");
var on_red = document.getElementById("img_red_on");
var on_red_br = document.getElementById("img_red_br_on");
var on_red_tc = document.getElementById("img_red_tc_on");
var glow_yellow = document.getElementById("img_yellow_glow");
var on_yellow = document.getElementById("img_yellow_on");
var on_yellow_br = document.getElementById("img_yellow_br_on");
var on_yellow_bl = document.getElementById("img_yellow_bl_on");
var on_yellow_tc = document.getElementById("img_yellow_tc_on");
var glow_green = document.getElementById("img_green_glow");
var on_green = document.getElementById("img_green_on");
var on_green_br = document.getElementById("img_green_br_on");
var on_green_bl = document.getElementById("img_green_bl_on");
var on_green_l = document.getElementById("img_green_l_on");
var glow_blue = document.getElementById("img_blue_glow");
var on_blue = document.getElementById("img_blue_on");

var seg_a = document.getElementById("img_7sega");
var seg_b = document.getElementById("img_7segb");
var seg_c = document.getElementById("img_7segc");
var seg_d = document.getElementById("img_7segd");
var seg_e = document.getElementById("img_7sege");
var seg_f = document.getElementById("img_7segf");
var seg_g = document.getElementById("img_7segg");

var sega_f = 1 << 0;
var segb_f = 1 << 1;
var segc_f = 1 << 2;
var segd_f = 1 << 3;
var sege_f = 1 << 4;
var segf_f = 1 << 5;
var segg_f = 1 << 6;

var seg_digits = [sega_f | segb_f | segc_f | segd_f | sege_f | segf_f, // 0
segb_f | segc_f, // 1
sega_f | segb_f | segd_f | sege_f | segg_f, // 2
sega_f | segb_f | segc_f | segd_f | segg_f, // 3
segb_f | segc_f | segf_f | segg_f, // 4
sega_f | segc_f | segd_f | segf_f | segg_f, // 5
sega_f | segc_f | segd_f | sege_f | segf_f | segg_f, // 6
sega_f | segb_f | segc_f, // 7
sega_f | segb_f | segc_f | segd_f | sege_f | segf_f | segg_f, // 8
sega_f | segb_f | segc_f | segd_f | segf_f | segg_f, // 9
sega_f | segb_f | segc_f | sege_f | segf_f | segg_f, // A
segc_f | segd_f | sege_f | segf_f | segg_f, // b
sega_f | segd_f | sege_f | segf_f, // C
segb_f | segc_f | segd_f | sege_f | segg_f, // d
sega_f | segd_f | sege_f | segf_f | segg_f, // E
sega_f | sege_f | segf_f | segg_f, // F
segb_f | segc_f | sege_f | segf_f | segg_f, // H
]

var ratio = 1.0,
  outw = 1.0,
  xoff = 0.0,
  yoff = 0.0,
  scale = 1.0;

window.addEventListener('resize', resizeCanvas, false);
document.onload = resizeCanvas;
img.onload = resizeCanvas;

var ledDefs = {
  ra: [
    { x: 1470, y: 1024 },
    { x: 1443, y: 1022 },
    { x: 1418, y: 1022 },
    { x: 1393, y: 1023 },
    { x: 1368, y: 1023 },
    { x: 1345, y: 1022 },
    { x: 1321, y: 1023 },
    { x: 1295, y: 1022 }
  ],

  rb: [
    { x: 1473, y: 644 },
    { x: 1448, y: 645 },
    { x: 1425, y: 645 },
    { x: 1397, y: 646 },
    { x: 1370, y: 646 },
    { x: 1347, y: 645 },
    { x: 1322, y: 644 },
    { x: 1294, y: 645 }
  ],

  rc: [
    { x: 1101, y: 239 },
    { x: 1077, y: 241 },
    { x: 1050, y: 241 },
    { x: 1024, y: 243 },
    { x: 1000, y: 241 },
    { x: 975, y: 241 },
    { x: 951, y: 240 },
    { x: 927, y: 240 }
  ],

  rd: [
    { x: 1629, y: 128 },
    { x: 1571, y: 129 },
    { x: 1514, y: 129 },
    { x: 1457, y: 129 }
  ],

  bus: [
    { x: 904, y: -11 },
    { x: 879, y: -9 },
    { x: 853, y: -6 },
    { x: 826, y: -7 },
    { x: 800, y: -6 },
    { x: 776, y: -6 },
    { x: 749, y: -5 },
    { x: 723, y: -8 }
  ],

  ma: [
    { x: 227, y: 405 },
    { x: 204, y: 407 },
    { x: 179, y: 406 },
    { x: 155, y: 406 },
    { x: 129, y: 406 },
    { x: 105, y: 405 },
    { x: 79, y: 404 },
    { x: 57, y: 406 }
  ],

  pgm: { x: 65, y: 362 },
  clk: { x: 573, y: 69 },
  clkmode: { x: 351, y: 164 },
  runMode: { x: 62, y: 608 },
  pgmMode: { x: 74, y: 590 },

  me: [
    { x: 727, y: 251 },
    { x: 702, y: 250 },
    { x: 677, y: 249 },
    { x: 651, y: 246 },
    { x: 627, y: 246 },
    { x: 602, y: 245 },
    { x: 578, y: 245 },
    { x: 553, y: 245 }
  ],

  alu: [
    { x: 1102, y: 638 },
    { x: 1074, y: 640 },
    { x: 1048, y: 642 },
    { x: 1022, y: 644 },
    { x: 998, y: 644 },
    { x: 974, y: 647 },
    { x: 948, y: 645 },
    { x: 925, y: 646 }
  ],

  aluf: [
    { x: 1259, y: 820 }, // n
    { x: 1285, y: 821 }, // o
    { x: 1233, y: 817 }, // c
    { x: 1207, y: 816 }, // z
  ],

  sp: [
    { x: 1091, y: 1011 },
    { x: 1066, y: 1014 },
    { x: 1040, y: 1014 },
    { x: 1017, y: 1013 },
    { x: 993, y: 1013 },
    { x: 969, y: 1012 },
    { x: 944, y: 1012 },
    { x: 916, y: 1011 }
  ],

  ir: [
    { x: 710, y: 854 },
    { x: 685, y: 855 },
    { x: 660, y: 855 },
    { x: 637, y: 855 },
    { x: 613, y: 858 },
    { x: 587, y: 859 },
    { x: 560, y: 858 },
    { x: 535, y: 857 }
  ],

  pc: [
    { x: 474, y: 854 },
    { x: 448, y: 854 },
    { x: 423, y: 854 },
    { x: 397, y: 856 },
    { x: 370, y: 854 },
    { x: 345, y: 853 },
    { x: 321, y: 852 },
    { x: 296, y: 852 }
  ],

  st: [
    { x: 153, y: 855 },
    { x: 133, y: 856 },
    { x: 114, y: 856 },
  ],

  cw_r: [
    { x: 1475, y: 1211 }, // ALU
    { x: 1424, y: 1210 }, // Rd
    { x: 1400, y: 1209 }, // Rc
    { x: 1373, y: 1208 }, // Rb
    { x: 1348, y: 1208 }, // Ra
    { x: 1322, y: 1208 }, // SP
    { x: 1246, y: 1207 }, // ME
    { x: 1147, y: 1206 }, // PC
  ],

  cw: [
    {}, {}, {},
    { x: 1604, y: 1225 }, // ALU0
    { x: 1575, y: 1227 },// ALU1
    { x: 1548, y: 1228 },// ALU2
    { x: 1525, y: 1227 },// ALUB
    { x: 1503, y: 1231 },// ALUCI
    { x: 1475, y: 1237 },// ALUW
    { x: 1424, y: 1232 },// Rd
    { x: 1398, y: 1232 },// Rc
    { x: 1373, y: 1233 },// Rb
    { x: 1348, y: 1234 },// Ra
    { x: 1321, y: 1232 },// SP
    { x: 1246, y: 1232 },// MW
    { x: 1218, y: 1218 },// PGM
    { x: 1296, y: 1232 }, // IR
    { x: 1118, y: 1216 }, // PCC
    { x: 1145, y: 1230 }, // PC
    { x: 1197, y: 1230 }, // MA
    {},
    {},
    { x: 1094, y: 1215 },  // TR
    { x: 1055, y: 1215 },  // HLT
  ]
};

var getXPos = function (x)
{
  return (x * scale) + xoff;
}

var getYPos = function (y)
{
  return (y * scale) + yoff;
}

var getXSize = function (xs)
{
  return (xs * scale);
}

var getYSize = function (ys)
{
  return (ys * scale);
}


function resizeCanvas()
{
  canv.width = window.innerWidth;
  canv.height = window.innerHeight - ($(".navbar").height() + 40);

  imgratio = $(img).width() / $(img).height();
  canvratio = canv.width / canv.height;

  if (imgratio < canvratio)
  {
    outw = imgratio * canv.height;
    xoff = (canv.width - outw) * 0.5;
    yoff = 0;
    scale = outw / $(img).width();
  }
  else
  {
    outh = canv.width / imgratio;
    yoff = (canv.height - outh) * 0.5;
    xoff = 0;
    scale = outh / $(img).height();
  }

  ctx.fillStyle = "#262626";
  ctx.fillRect(0, 0, canv.width, canv.height);

}
resizeCanvas();


var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');

var getMousePos = function (evt)
{
  var rect = canv.getBoundingClientRect();
  return {
    x: evt.clientX - rect.left,
    y: evt.clientY - rect.top
  };
}

var getMouseDist = function (evt, target)
{
  var pos = getMousePos(evt);
  var dx = Math.abs(pos.x - getXPos(target.x));
  var dy = Math.abs(pos.y - getYPos(target.y));
  return Math.sqrt(dx * dx + dy * dy);
}

var Component = {
  Ra: 0,
  Rb: 1,
  Rc: 2,
  Rd: 3,
  SP: 4,
  PC: 5,
  IR: 6,
  TR: 7,
  MA: 8,
  ME: 9,
  AL: 10,
  FL: 11,
  BU: 12,
};

lcdModuleBackup = vrEmuLcdModule;

Module = {};

vrEmuLcdModule = Module;

Module.onRuntimeInitialized = function ()
{

  simLib = {
    initialise: Module.cwrap('simLibInitialise', null),
    destroy: Module.cwrap('simLibDestroy', null),
    loadProgram: Module.cwrap('simLibLoadProgram', null, ['string']),
    loadRam: Module.cwrap('simLibLoadRam', null, ['string']),
    ramByte: Module.cwrap('simLibRamByte', 'number', ['number']),
    setClock: Module.cwrap('simLibSetClock', null, ['number']),
    reset: Module.cwrap('simLibReset', null),
    getValue: Module.cwrap('simLibGetValue', 'number', ['number']),
    getLcd: Module.cwrap('simLibGetLcd', 'number'),
    getControlWord: Module.cwrap('simLibGetControlWord', 'number'),
  };

  lcdModuleBackup.onRuntimeInitialized();

  simLib.initialise();

  lcd = vrEmuLcd.registerLcd(simLib.getLcd());
  //lcd.colorScheme = vrEmuLcd.Schemes.GreenBlack;

  var programHex = getParam("h");
  if (!programHex) 
  {
    programHex = "37c1cf3f012f00";
  }

  var ramData = programHex.substring(512);

  simLib.loadProgram(programHex.substring(0, 512));
  
  if (ramData) 
  {
    simLib.loadRam(ramData);
  }

  var tick = 0;
  var lastTick = 0;
  var lastD = 0;
  var autoClock = true;
  var speed = parseInt(getParam("s"));
  if (!speed) speed = 100;
  if (speed < 0) speed = 1;
  if (speed > 500) speed = 500;
  
  var clkMode = { x: 280, y: 173 }
  var step = { x: 188, y: 188 }
  var reset = { x: 89, y: 986 }
  var spdUp = { x: 135, y: 124 }
  var spdDn = { x: 104, y: 188 }
  var dispNeg = { x: 1376, y: 91 }
  var dispHex = { x: 1320, y: 89 }

  var dispMode = 0; // unsigned

  var isResetting = false;

  canv.addEventListener('click', function (evt)
  {
    if (getMouseDist(evt, clkMode) < getXSize(30))
    {
      autoClock = !autoClock;
      if (!autoClock && (tick % 2))++tick;
    }
    else if (getMouseDist(evt, spdUp) < getXSize(30))
    {
      if (speed < 500)
      {
        speed += 20;
      }
    }
    else if (getMouseDist(evt, spdDn) < getXSize(30))
    {
      if (speed > 0)
      {
        speed -= 20;
      }
    }
    else if (getMouseDist(evt, dispNeg) < getXSize(30))
    {
      if (dispMode == 1)
        dispMode = 0;
      else
        dispMode = 1;
    }
    else if (getMouseDist(evt, dispHex) < getXSize(30))
    {
      if (dispMode == 2)
        dispMode = 0;
      else
        dispMode = 2;
    }
  }, false);

  var setCursor = function (evt)
  {
   canv.style.cursor = "grab";
    if (getMouseDist(evt, clkMode) < getXSize(30))
    {
      canv.title = "Clock mode (auto/manual).";
    }
    else if (getMouseDist(evt, step) < getXSize(30))
    {
      canv.title = "Manual step.";
    }
    else if (getMouseDist(evt, reset) < getXSize(30))
    {
      canv.title = "Reset program counter.";
    }
    else if (getMouseDist(evt, spdUp) < getXSize(30))
    {
      canv.title = "Increase clock speed.";
    }
    else if (getMouseDist(evt, spdDn) < getXSize(30))
    {
      canv.title = "Decrease clock speed.";
    }
    else if (getMouseDist(evt, dispNeg) < getXSize(30))
    {
      canv.title = "Toggle display (signed/unsigned)";
    }
    else if (getMouseDist(evt, dispHex) < getXSize(30))
    {
      canv.title = "Toggle display (hex/dec)";
    }
    else
    {
      canv.title="";
      canv.style.cursor = "default";
    }
  }

  canv.addEventListener('mousemove', function (evt)
  {
    setCursor(evt);
  }, false);


  canv.addEventListener('mousedown', function (evt)
  {
    if (getMouseDist(evt, step) < getXSize(30))
    {
      if (tick % 2 == 0)++tick;
    }
    else if (getMouseDist(evt, reset) < getXSize(30))
    {
      isResetting = true;
    }

    if (canv.style.cursor == "grab")
    {
      canv.style.cursor = "grabbing";
    }
  }, false);

  canv.addEventListener('mouseup', function (evt)
  {
    if (getMouseDist(evt, step) < getXSize(30))
    {
      if (tick % 2)++tick;
    }

    isResetting = false;

    setCursor(evt);
  }, false);

  var drawScaled = function(img, x, y, w, h)
  {
    if (img.complete && img.naturalHeight !== 0)
    {
      ctx.drawImage(img, getXPos(x), getYPos(y), getXSize(w), getYSize(h));
    }
  }

  var renderByte = function (img, val, arr, sz)
  {
    if (!sz) sz = 100;
    for (var a = 0; a < arr.length; ++a)
    {
      if (val & (1 << a))
      {
        drawScaled(img, arr[a].x, arr[a].y, sz, sz);
      }
    }
  }
  
  var wasRunning = true;
  var loop = function ()
  {
    var stepsPerCycle = 0;
    if (autoClock && (speed > 150))
    {
        stepsPerCycle = (speed - 150) + 1;
    }

    for (var i = stepsPerCycle; i >=0; --i)
    {

      if (autoClock)
      {
        ++tick;
      }
      if (lastTick != tick)
      {
        simLib.setClock(tick % 2);
        lastTick = tick;
      }
      
      if (i > 0)
      {
          // only render on the last cycle
          continue;
      }
      
      ctx.fillStyle = "#262626f0";
      ctx.fillRect(0, 0, canv.width, canv.height);
      drawScaled(img, 0, 0, $(img).width(), $(img).height());
      
      var cwv = simLib.getControlWord();
      var isRunning = (cwv & (1 << 23)) == 0;

      if (isRunning != wasRunning)
      {
        var ramImage = ""
        for (var i = 0; i < 256; ++i)
        {
          var b = simLib.ramByte(i);
          ramImage += b.toString(16).padStart(2, '0');
        }
        console.log(ramImage);
        console.log(tick);
      }
      wasRunning = isRunning;

      if (isResetting)
      {
        simLib.reset();
      }

      var rav = simLib.getValue(Component.Ra);
      renderByte(glow_red, rav, ledDefs.ra);
      renderByte(on_red_br, rav, ledDefs.ra);

      var rbv = simLib.getValue(Component.Rb);
      renderByte(glow_red, rbv, ledDefs.rb);
      renderByte(on_red, rbv, ledDefs.rb);

      var rcv = simLib.getValue(Component.Rc);
      renderByte(glow_red, rcv, ledDefs.rc);
      renderByte(on_red_tc, rcv, ledDefs.rc);

      var segSizeX = 60;
      var segSizeY = 77;

      var rdv = simLib.getValue(Component.Rd);
      if (dispMode == 1)
      {
        if (rdv & 0x80)
        {
          rdv = Math.abs(256 - rdv);
          drawScaled(seg_g, ledDefs.rd[3].x, ledDefs.rd[3].y, segSizeX, segSizeY);
        }
      }

      if (dispMode == 2)
      {
        var hex = rdv.toString(16).padStart(3, '0');
        for (var dig = 0; dig < 3; ++dig)
        {
          var digit = hex.charCodeAt(hex.length - (dig + 1));
          if (dig == 2)
          {
            digit = 16;
          }
          else if (digit >= 97) digit -= 87;
          else if (digit >= 65) digit -= 55;
          else digit -= 48;

          var segs = seg_digits[digit];
          if (segs & sega_f) drawScaled(seg_a, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segb_f) drawScaled(seg_b, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segc_f) drawScaled(seg_c, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segd_f) drawScaled(seg_d, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & sege_f) drawScaled(seg_e, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segf_f) drawScaled(seg_f, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segg_f) drawScaled(seg_g, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
        }
      }
      else
      {
        for (var dig = 0; dig < 3; ++dig)
        {
          var digit = Math.floor((rdv / Math.pow(10, dig))) % 10;
          var segs = seg_digits[digit];
          if (segs & sega_f) drawScaled(seg_a, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segb_f) drawScaled(seg_b, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segc_f) drawScaled(seg_c, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segd_f) drawScaled(seg_d, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & sege_f) drawScaled(seg_e, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segf_f) drawScaled(seg_f, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
          if (segs & segg_f) drawScaled(seg_g, ledDefs.rd[dig].x, ledDefs.rd[dig].y, segSizeX, segSizeY);
        }
      }

      var spv = simLib.getValue(Component.SP);
      renderByte(glow_yellow, spv, ledDefs.sp);
      renderByte(on_yellow_br, spv, ledDefs.sp);

      var pcv = simLib.getValue(Component.PC);
      renderByte(glow_green, pcv, ledDefs.pc);
      renderByte(on_green_bl, pcv, ledDefs.pc);

      var stv = simLib.getValue(Component.TR);
      renderByte(glow_green, stv, ledDefs.st, 60);
      renderByte(on_green_bl, stv, ledDefs.st, 60);

      var irv = simLib.getValue(Component.IR);
      renderByte(glow_yellow, irv, ledDefs.ir);
      renderByte(on_yellow_bl, irv, ledDefs.ir);

      var mav = simLib.getValue(Component.MA);
      renderByte(glow_yellow, mav, ledDefs.ma);
      renderByte(on_yellow_br, mav, ledDefs.ma);

      var mev = simLib.getValue(Component.ME);
      renderByte(glow_red, mev, ledDefs.me);
      renderByte(on_red_br, mev, ledDefs.me);

      var aluv = simLib.getValue(Component.AL);
      renderByte(glow_green, aluv, ledDefs.alu);
      renderByte(on_green_br, aluv, ledDefs.alu);

      var flv = simLib.getValue(Component.FL);
      renderByte(glow_yellow, flv, ledDefs.aluf);
      renderByte(on_yellow_br, flv, ledDefs.aluf);

      var buv = simLib.getValue(Component.BU);
      renderByte(glow_red, buv, ledDefs.bus);
      renderByte(on_red_tc, buv, ledDefs.bus);

      if (autoClock)
      {
        drawScaled(glow_green, ledDefs.clkmode.x, ledDefs.clkmode.y, 100, 100);
        drawScaled(on_green_br, ledDefs.clkmode.x, ledDefs.clkmode.y, 100, 100);
      }

      drawScaled(glow_green, ledDefs.runMode.x, ledDefs.runMode.y, 100, 100);
      drawScaled(on_green_l, ledDefs.runMode.x, ledDefs.runMode.y, 100, 100);


      if (((stepsPerCycle > 0 && (tick % 7) != 0) || (tick % 2)) && isRunning)
      {
        drawScaled(glow_blue, ledDefs.clk.x, ledDefs.clk.y, 100, 100);
        drawScaled(on_blue, ledDefs.clk.x, ledDefs.clk.y, 100, 100);
      }


      if (cwv & (1 << 15))
      {
        drawScaled(glow_green, ledDefs.pgm.x, ledDefs.pgm.y, 100, 100);
        drawScaled(on_green_br, ledDefs.pgm.x, ledDefs.pgm.y, 100, 100);
      }

      var bwv = 1 << (cwv & 0x07);
      renderByte(glow_green, bwv, ledDefs.cw_r);
      renderByte(on_green_br, bwv, ledDefs.cw_r);


      for (var cwi = 0; cwi < ledDefs.cw.length; ++cwi)
      {
        if (!ledDefs.cw[cwi].x)
        {
          continue;
        }

        var on = false;
        switch (cwi)
        {
          case 8:
          case 9:
          case 10:
          case 11:
          case 12:
          case 13:
          case 14:
          case 16:
          case 18:
          case 19:
            on = (cwv & (1 << cwi)) == 0;
            break;

          default:
            on = (cwv & (1 << cwi)) != 0;
            break;
        }

        if (on)
        {
          drawScaled(glow_green, ledDefs.cw[cwi].x, ledDefs.cw[cwi].y, 100, 100);
          drawScaled(on_green_br, ledDefs.cw[cwi].x, ledDefs.cw[cwi].y, 100, 100);
        }
      }
      
      lcd.render(ctx, getXPos(340), getYPos(780), getXSize(300), getYSize(106));
      drawScaled(lcdimg, 313, 711, 674, 177);
      
    }
    window.setTimeout(loop, speed > 160 ? 0 : (200 / (speed <= 0 ? 1 : speed)));
  };

  loop();
};
