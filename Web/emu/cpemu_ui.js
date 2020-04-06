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
var glow_yellow = document.getElementById("img_yellow_glow");
var on_yellow = document.getElementById("img_yellow_on");
var glow_green = document.getElementById("img_green_glow");
var on_green = document.getElementById("img_green_on");
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

var ledDefs = {
  ra: [
    { x: 1127, y: 774 },
    { x: 1109, y: 775 },
    { x: 1088, y: 775 },
    { x: 1069, y: 774 },
    { x: 1049, y: 774 },
    { x: 1029, y: 776 },
    { x: 1010, y: 777 },
    { x: 990, y: 779 }
  ],

  rb: [
    { x: 1126, y: 480 },
    { x: 1106, y: 480 },
    { x: 1087, y: 480 },
    { x: 1067, y: 480 },
    { x: 1047, y: 480 },
    { x: 1028, y: 479 },
    { x: 1008, y: 478 },
    { x: 990, y: 478 }
  ],

  rc: [
    { x: 833, y: 158 },
    { x: 814, y: 159 },
    { x: 794, y: 160 },
    { x: 774, y: 161 },
    { x: 753, y: 160 },
    { x: 733, y: 159 },
    { x: 714, y: 158 },
    { x: 695, y: 158 }
  ],

  rd: [
    { x: 1244, y: 89 },
    { x: 1201, y: 89 },
    { x: 1158, y: 89 },
    { x: 1114, y: 88 }
  ],

  bus: [
    { x: 680, y: -37 },
    { x: 658, y: -35 },
    { x: 637, y: -32 },
    { x: 616, y: -33 },
    { x: 596, y: -33 },
    { x: 578, y: -32 },
    { x: 557, y: -34 },
    { x: 537, y: -33 }
  ],

  ma: [
    { x: 142, y: 291 },
    { x: 124, y: 293 },
    { x: 106, y: 293 },
    { x: 88, y: 293 },
    { x: 69, y: 294 },
    { x: 50, y: 295 },
    { x: 32, y: 295 },
    { x: 13, y: 296 }
  ],

  pgm: { x: 21, y: 253 },
  clk: { x: 412, y: 28 },
  clkmode: { x: 239, y: 110 },
  runMode: { x: 20, y: 457 },
  pgmMode: { x: 16, y: 435 },

  me: [
    { x: 534, y: 164 },
    { x: 514, y: 164 },
    { x: 495, y: 164 },
    { x: 474, y: 163 },
    { x: 455, y: 163 },
    { x: 436, y: 163 },
    { x: 416, y: 164 },
    { x: 397, y: 165 }
  ],

  alu: [
    { x: 836, y: 477 },
    { x: 815, y: 478 },
    { x: 794, y: 478 },
    { x: 774, y: 479 },
    { x: 754, y: 479 },
    { x: 734, y: 481 },
    { x: 713, y: 480 },
    { x: 694, y: 480 }
  ],

  aluf: [
    { x: 962, y: 619 }, // n
    { x: 983, y: 619 }, // o
    { x: 943, y: 618 }, // c
    { x: 923, y: 617 }, // z
  ],

  sp: [
    { x: 831, y: 777 },
    { x: 811, y: 778 },
    { x: 791, y: 779 },
    { x: 771, y: 779 },
    { x: 751, y: 779 },
    { x: 731, y: 778 },
    { x: 710, y: 779 },
    { x: 689, y: 779 }
  ],

  ir: [
    { x: 522, y: 656 },
    { x: 502, y: 659 },
    { x: 481, y: 657 },
    { x: 461, y: 656 },
    { x: 441, y: 657 },
    { x: 422, y: 657 },
    { x: 401, y: 657 },
    { x: 380, y: 657 }
  ],

  pc: [
    { x: 335, y: 649 },
    { x: 315, y: 649 },
    { x: 295, y: 649 },
    { x: 274, y: 650 },
    { x: 253, y: 650 },
    { x: 233, y: 649 },
    { x: 212, y: 650 },
    { x: 193, y: 651 }
  ],

  st: [
    { x: 81, y: 668 },
    { x: 68, y: 666 },
    { x: 56, y: 666 },
  ],

  cw_r: [
    { x: 1130, y: 927 }, // ALU
    { x: 1092, y: 927 }, // Rd
    { x: 1073, y: 927 }, // Rc
    { x: 1053, y: 927 }, // Rb
    { x: 1033, y: 927 }, // Ra
    { x: 1013, y: 927 }, // SP
    { x: 954, y: 927 }, // ME
    { x: 874, y: 930 }, // PC
  ],

  cw: [
    {}, {}, {},
    { x: 1229, y: 935 }, // ALU0
    { x: 1207, y: 936 },// ALU1
    { x: 1187, y: 938 },// ALU2
    { x: 1169, y: 937 },// ALUB
    { x: 1151, y: 937 },// ALUCI
    { x: 1132, y: 948 },// ALUW
    { x: 1093, y: 947 },// Rd
    { x: 1074, y: 948 },// Rc
    { x: 1055, y: 949 },// Rb
    { x: 1034, y: 949 },// Ra
    { x: 1014, y: 948 },// SP
    { x: 955, y: 948 },// MW
    { x: 936, y: 943 },// PGM
    { x: 994, y: 947 }, // IR
    { x: 854, y: 938 }, // PCC
    { x: 873, y: 949 }, // PC
    { x: 915, y: 948 }, // MA
    {},
    {},
    { x: 833, y: 937 },  // TR
    { x: 801, y: 938 },  // HLT
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

  ratio = $(img).width() / $(img).height();
  outw = ratio * $(canv).height();
  xoff = ($(canv).width() - outw) * 0.5;
  scale = outw / $(img).width();

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
  var dx = Math.abs(pos.x - target.x);
  var dy = Math.abs(pos.y - target.y);
  return Math.sqrt(dx * dx + dy * dy);
}


var renderByte = function (img, val, arr, sz)
{
  if (!sz) sz = 100;
  for (var a = 0; a < arr.length; ++a)
  {
    if (val & (1 << a))
    {
      ctx.drawImage(img, getXPos(arr[a].x), getYPos(arr[a].y), getXSize(sz), getYSize(sz));
    }
  }
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
    setClock: Module.cwrap('simLibSetClock', null, ['number']),
    reset: Module.cwrap('simLibReset', null),
    getValue: Module.cwrap('simLibGetValue', 'number', ['number']),
    getLcd: Module.cwrap('simLibGetLcd', 'number'),
    getControlWord: Module.cwrap('simLibGetControlWord', 'number'),
  };

  lcdModuleBackup.onRuntimeInitialized();

  simLib.initialise();

  lcd = vrEmuLcd.registerLcd(simLib.getLcd());

  var programHex = getParam("h");
  if (!programHex) 
  {
    programHex = "37c1cf3f012f00";
  }

  simLib.loadProgram(programHex);
  var tick = 0;
  var lastTick = 0;
  var lastD = 0;
  var autoClock = true;
  var speed = parseInt(getParam("s"));
  if (!speed) speed = 100;
  if (speed < 0) speed = 1;
  if (speed > 300) speed = 300;
  
  console.log(speed);

  var clkMode = { x: 210, y: 132 }
  var step = { x: 131, y: 137 }
  var reset = { x: 40, y: 764 }
  var spdUp = { x: 85, y: 94 }
  var spdDn = { x: 85, y: 146 }
  var dispNeg = { x: 1050, y: 61 }
  var dispHex = { x: 1027, y: 63 }

  var dispMode = 0; // unsigned

  var isResetting = false;

  canv.addEventListener('click', function (evt)
  {
    if (getMouseDist(evt, { x: getXPos(clkMode.x), y: getYPos(clkMode.y) }) < 10)
    {
      autoClock = !autoClock;
      if (!autoClock && (tick % 2))++tick;
    }
    else if (getMouseDist(evt, { x: getXPos(spdUp.x), y: getYPos(spdUp.y) }) < 10)
    {
      if (speed < 300)
      {
        speed += 20;
      }
    }
    else if (getMouseDist(evt, { x: getXPos(spdDn.x), y: getYPos(spdDn.y) }) < 10)
    {
      if (speed > 0)
      {
        speed -= 20;
      }
    }
    else if (getMouseDist(evt, { x: getXPos(dispNeg.x), y: getYPos(dispNeg.y) }) < 10)
    {
      if (dispMode == 1)
        dispMode = 0;
      else
        dispMode = 1;
    }
    else if (getMouseDist(evt, { x: getXPos(dispHex.x), y: getYPos(dispHex.y) }) < 10)
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
    if (getMouseDist(evt, { x: getXPos(clkMode.x), y: getYPos(clkMode.y) }) < 10)
    {
      canv.title = "Clock mode (auto/manual).";
    }
    else if (getMouseDist(evt, { x: getXPos(step.x), y: getYPos(step.y) }) < 10)
    {
      canv.title = "Manual step.";
    }
    else if (getMouseDist(evt, { x: getXPos(reset.x), y: getYPos(reset.y) }) < 10)
    {
      canv.title = "Reset program counter.";
    }
    else if (getMouseDist(evt, { x: getXPos(spdUp.x), y: getYPos(spdUp.y) }) < 10)
    {
      canv.title = "Increase clock speed.";
    }
    else if (getMouseDist(evt, { x: getXPos(spdDn.x), y: getYPos(spdDn.y) }) < 10)
    {
      canv.title = "Decrease clock speed.";
    }
    else if (getMouseDist(evt, { x: getXPos(dispNeg.x), y: getYPos(dispNeg.y) }) < 10)
    {
      canv.title = "Toggle display (signed/unsigned)";
    }
    else if (getMouseDist(evt, { x: getXPos(dispHex.x), y: getYPos(dispHex.y) }) < 10)
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
    if (getMouseDist(evt, { x: getXPos(step.x), y: getYPos(step.y) }) < 10)
    {
      if (tick % 2 == 0)++tick;
    }
    else if (getMouseDist(evt, { x: getXPos(reset.x), y: getYPos(reset.y) }) < 10)
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
    if (getMouseDist(evt, { x: getXPos(step.x), y: getYPos(step.y) }) < 10)
    {
      if (tick % 2)++tick;
    }

    isResetting = false;

    setCursor(evt);
  }, false);

  var loop = function ()
  {
    var stepsPerCycle = 1;
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
      ctx.drawImage(img, xoff, yoff, scale * $(img).width(), scale * $(img).height());
      //console.log(tick);

      if (isResetting)
      {
        simLib.reset();
      }

      var rav = simLib.getValue(Component.Ra);
      renderByte(glow_red, rav, ledDefs.ra);
      renderByte(on_red, rav, ledDefs.ra);

      var rbv = simLib.getValue(Component.Rb);
      renderByte(glow_red, rbv, ledDefs.rb);
      renderByte(on_red, rbv, ledDefs.rb);

      var rcv = simLib.getValue(Component.Rc);
      renderByte(glow_red, rcv, ledDefs.rc);
      renderByte(on_red, rcv, ledDefs.rc);

      var rdv = simLib.getValue(Component.Rd);
      if (dispMode == 1)
      {
        if (rdv & 0x80)
        {
          rdv = Math.abs(256 - rdv);
          ctx.drawImage(seg_g, getXPos(ledDefs.rd[3].x), getYPos(ledDefs.rd[3].y), getXSize(42), getYSize(59));
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
          if (segs & sega_f) ctx.drawImage(seg_a, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segb_f) ctx.drawImage(seg_b, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segc_f) ctx.drawImage(seg_c, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segd_f) ctx.drawImage(seg_d, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & sege_f) ctx.drawImage(seg_e, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segf_f) ctx.drawImage(seg_f, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segg_f) ctx.drawImage(seg_g, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
        }
      }
      else
      {
        for (var dig = 0; dig < 3; ++dig)
        {
          var digit = Math.floor((rdv / Math.pow(10, dig))) % 10;
          var segs = seg_digits[digit];
          if (segs & sega_f) ctx.drawImage(seg_a, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segb_f) ctx.drawImage(seg_b, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segc_f) ctx.drawImage(seg_c, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segd_f) ctx.drawImage(seg_d, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & sege_f) ctx.drawImage(seg_e, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segf_f) ctx.drawImage(seg_f, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
          if (segs & segg_f) ctx.drawImage(seg_g, getXPos(ledDefs.rd[dig].x), getYPos(ledDefs.rd[dig].y), getXSize(42), getYSize(59));
        }
      }

      var spv = simLib.getValue(Component.SP);
      renderByte(glow_yellow, spv, ledDefs.sp);
      renderByte(on_yellow, spv, ledDefs.sp);

      var pcv = simLib.getValue(Component.PC);
      renderByte(glow_green, pcv, ledDefs.pc);
      renderByte(on_green, pcv, ledDefs.pc);

      var stv = simLib.getValue(Component.TR);
      renderByte(glow_green, stv, ledDefs.st, 50);
      renderByte(on_green, stv, ledDefs.st, 50);

      var irv = simLib.getValue(Component.IR);
      renderByte(glow_yellow, irv, ledDefs.ir);
      renderByte(on_yellow, irv, ledDefs.ir);

      var mav = simLib.getValue(Component.MA);
      renderByte(glow_yellow, mav, ledDefs.ma);
      renderByte(on_yellow, mav, ledDefs.ma);

      var mev = simLib.getValue(Component.ME);
      renderByte(glow_red, mev, ledDefs.me);
      renderByte(on_red, mev, ledDefs.me);

      var aluv = simLib.getValue(Component.AL);
      renderByte(glow_green, aluv, ledDefs.alu);
      renderByte(on_green, aluv, ledDefs.alu);

      var flv = simLib.getValue(Component.FL);
      renderByte(glow_yellow, flv, ledDefs.aluf);
      renderByte(on_yellow, flv, ledDefs.aluf);

      var buv = simLib.getValue(Component.BU);
      renderByte(glow_red, buv, ledDefs.bus);
      renderByte(on_red, buv, ledDefs.bus);

      if (autoClock)
      {
        ctx.drawImage(glow_green, getXPos(ledDefs.clkmode.x), getYPos(ledDefs.clkmode.y), getXSize(100), getYSize(100));
        ctx.drawImage(on_green, getXPos(ledDefs.clkmode.x), getYPos(ledDefs.clkmode.y), getXSize(100), getYSize(100));
      }

      ctx.drawImage(glow_green, getXPos(ledDefs.runMode.x), getYPos(ledDefs.runMode.y), getXSize(100), getYSize(100));
      ctx.drawImage(on_green, getXPos(ledDefs.runMode.x), getYPos(ledDefs.runMode.y), getXSize(100), getYSize(100));

      var cwv = simLib.getControlWord();

      if ((tick % 2) && (cwv & (1 << 23)) == 0)
      {
        ctx.drawImage(glow_blue, getXPos(ledDefs.clk.x), getYPos(ledDefs.clk.y), getXSize(100), getYSize(100));
        ctx.drawImage(on_blue, getXPos(ledDefs.clk.x), getYPos(ledDefs.clk.y), getXSize(100), getYSize(100));
      }


      if (cwv & (1 << 15))
      {
        ctx.drawImage(glow_green, getXPos(ledDefs.pgm.x), getYPos(ledDefs.pgm.y), getXSize(100), getYSize(100));
        ctx.drawImage(on_green, getXPos(ledDefs.pgm.x), getYPos(ledDefs.pgm.y), getXSize(100), getYSize(100));
      }

      var bwv = 1 << (cwv & 0x07);
      renderByte(glow_green, bwv, ledDefs.cw_r);
      renderByte(on_green, bwv, ledDefs.cw_r);


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
          ctx.drawImage(glow_green, getXPos(ledDefs.cw[cwi].x), getYPos(ledDefs.cw[cwi].y), getXSize(100), getYSize(100));
          ctx.drawImage(on_green, getXPos(ledDefs.cw[cwi].x), getYPos(ledDefs.cw[cwi].y), getXSize(100), getYSize(100));
        }
      }
      
      lcd.render(ctx, getXPos(240), getYPos(588), getXSize(239), getYSize(76));
      ctx.drawImage(lcdimg, getXPos(215), getYPos(550), getXSize(292), getYSize(126));
      
    }
    window.setTimeout(loop, speed > 160 ? 0 : (200 / (speed <= 0 ? 1 : speed)));
  };

  loop();
};
