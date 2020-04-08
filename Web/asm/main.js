let g_wasm = null
let g_codeEditor = null


function main() {
  setupEditor()
  onResize()

  document.body.onresize = onResize
  window.onkeydown = onKeyDown
  window.onbeforeunload = onBeforeUnload

  fetch("customasm.gc.wasm")
    .then(r => r.arrayBuffer())
    .then(r => WebAssembly.instantiate(r))
    .then(wasm => {
      g_wasm = wasm
      document.getElementById("buttonAssemble").disabled = false
    })
}

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

function loadProgramFile(file) {
  fetch(file)
    .then(r => r.text())
    .then(r => g_codeEditor.setValue(r))

  g_codeEditor.refresh()
}


function setupEditor() {
  g_codeEditor = CodeMirror(document.getElementById("divCodeMirror"), {
    lineNumbers: true,
    matchBrackets: true,
    indentWithTabs: true,
    highlightSelectionMatches: true,
    tabSize: 4,
    indentUnit: 4,
    mode: "z80"
  })

  g_codeEditor.setOption("theme", "lesser-dark");
  
  var example = getParam("e");
  if (!example)
  {
	  example = "Fibonacci";
  }

  loadProgramFile("examples/"  + example + ".asm");
}


function onResize() {
  let rectInput = document.getElementById("divCodeMirror").getBoundingClientRect()
  g_codeEditor.setSize(rectInput.width, rectInput.height)
}


function onBeforeUnload() {
  return "Your work will be lost if you close the page."
}


function onKeyDown(ev) {
  if (!ev.ctrlKey)
    return

  if (ev.key == "Enter") {
    ev.preventDefault()
    assemble()
  }
}

function hex2bin(hex) {
  return (parseInt(hex, 16).toString(2)).padStart(8, '0');
}

lineOffset = 0;

function doAssemble(format, success) {
  if (g_wasm == null)
    return;

  fetch("troyscpudef.asm")
    .then(r => r.text())
    .then(function (r) {
      var code = "";
      code = r; //.text()
      lineOffset = (code.split(/\r\n|\r|\n/).length)

      code = code + "\n" + g_codeEditor.getValue();

      let asmPtr = makeRustString(code); //g_codeEditor.getValue())
      let outputPtr = null
      try {
        outputPtr = g_wasm.instance.exports.wasm_assemble(format, asmPtr)
      } catch (e) {
        alert("Error assembling!\n\n" + e)
        throw e
      }

      let output = readRustString(outputPtr)

      dropRustString(asmPtr)
      dropRustString(outputPtr)

      success(output)
    });
}

function submit() {
  $("#buttonSubmit")
    .addClass("btn-secondary")
    .removeClass("btn-success")
    .prop("disabled", true);

  doAssemble(4, function (output) {
    $.post("submit/", {
      code: output
    }, function (data) {
      alert("Program submitted successfully.");
    });

  });
}

var emulator = null;

function emulate() {
  doAssemble(4, function (output) {
    var newUrl = "../emu/?h=" + output + "&s=" + getParam("s");
    if (emulator && emulator.location.href) {
      emulator.location.href = newUrl;
      emulator.focus();
    } else {
      emulator = window.open(newUrl);
    }

  });
}

function loadExample(file) {
  loadProgramFile(file);
}

function assemble() {
  doAssemble(1, function (output) {
    output = output.replace("outp | addr |", "    address    |")
    output = output.replace(/\n.*\|(.*)\|/g, function (match, p1, offset, string) {
      return "\n " + parseInt(p1, 16).toString().padEnd(4, ' ') + ": " + hex2bin(p1) + " |";
    });
    output = output.replace(/                            /g, "          ");
    output = output.replace(/\n([^\|]+)\|\s+\;/g, "\n                |                            ;")

    output = output.replace(/\n/g, "<br>")
    output = output.replace(/\x1b\[0m\x1b\[90m/g, "</span><span style='color:gray;'>")
    output = output.replace(/\x1b\[0m\x1b\[91m/g, "</span><span style='color:red;'>")
    output = output.replace(/\x1b\[0m\x1b\[93m/g, "</span><span style='color:#f80;'>")
    output = output.replace(/\x1b\[0m\x1b\[97m/g, "</span><span style='color:EBEFE7;'>")
    output = output.replace(/\x1b\[0m/g, "</span><span style='color:EBEFE7;'>")

    if (output.indexOf("asm:") >= 0) {
      output = output.replace(/(\d+)/g, function (match, p1, offset, string) {
        var val = parseInt(p1);
        if (val >= lineOffset) {
          return (val - lineOffset).toString().padEnd(p1.length, ' ');
        }
        return p1;
      })
      $("#buttonSubmit")
        .addClass("btn-secondary")
        .removeClass("btn-success")
        .prop("disabled", true);
      $("#buttonEmulate")
        .addClass("btn-secondary")
        .removeClass("btn-primary")
        .prop("disabled", true);
    } else {
      $("#buttonSubmit")
        .addClass("btn-success")
        .removeClass("btn-secondary")
        .prop("disabled", false);
      $("#buttonEmulate")
        .addClass("btn-primary")
        .removeClass("btn-secondary")
        .prop("disabled", false);
    }

    output = "<span style='color:EBEFE7;'>" + output + "</span>"

    let isError = output.includes("error")

    let divText = document.getElementById("divOutputText")
    divText.innerHTML = output
    divText.style.whiteSpace = "no-wrap"

  });
}


function makeRustString(str) {
  //console.log("makeRustString")
  //console.log(str)

  let bytes = window.TextEncoder ? new TextEncoder("utf-8").encode(str) : stringToUtf8ByteArray(str)
  //console.log(bytes)

  let ptr = g_wasm.instance.exports.wasm_string_new(bytes.length)

  for (let i = 0; i < bytes.length; i++)
    g_wasm.instance.exports.wasm_string_set_byte(ptr, i, bytes[i])

  //console.log(ptr)
  return ptr
}


function readRustString(ptr) {
  //console.log("readRustString")
  //console.log(ptr)

  let len = g_wasm.instance.exports.wasm_string_get_len(ptr)
  //console.log(len)

  let bytes = []
  for (let i = 0; i < len; i++)
    bytes.push(g_wasm.instance.exports.wasm_string_get_byte(ptr, i))

  //console.log(bytes)

  let str = window.TextDecoder ? new TextDecoder("utf-8").decode(new Uint8Array(bytes)) : utf8ByteArrayToString(bytes)
  //console.log(str)
  return str
}


function dropRustString(ptr) {
  //console.log("dropRustString")
  //console.log(ptr)

  g_wasm.instance.exports.wasm_string_drop(ptr)
}


// From https://github.com/google/closure-library/blob/e877b1eac410c0d842bcda118689759512e0e26f/closure/goog/crypt/crypt.js#L115
function stringToUtf8ByteArray(str) {
  let out = [],
    p = 0
  for (let i = 0; i < str.length; i++) {
    let c = str.charCodeAt(i)
    if (c < 128) {
      out[p++] = c
    } else if (c < 2048) {
      out[p++] = (c >> 6) | 192
      out[p++] = (c & 63) | 128
    } else if (
      ((c & 0xFC00) == 0xD800) && (i + 1) < str.length &&
      ((str.charCodeAt(i + 1) & 0xFC00) == 0xDC00)) {
      // Surrogate Pair
      c = 0x10000 + ((c & 0x03FF) << 10) + (str.charCodeAt(++i) & 0x03FF)
      out[p++] = (c >> 18) | 240
      out[p++] = ((c >> 12) & 63) | 128
      out[p++] = ((c >> 6) & 63) | 128
      out[p++] = (c & 63) | 128
    } else {
      out[p++] = (c >> 12) | 224
      out[p++] = ((c >> 6) & 63) | 128
      out[p++] = (c & 63) | 128
    }
  }
  return out
}


// From https://github.com/google/closure-library/blob/e877b1eac410c0d842bcda118689759512e0e26f/closure/goog/crypt/crypt.js#L149
function utf8ByteArrayToString(bytes) {
  let out = [],
    pos = 0,
    c = 0
  while (pos < bytes.length) {
    let c1 = bytes[pos++]
    if (c1 < 128) {
      out[c++] = String.fromCharCode(c1)
    } else if (c1 > 191 && c1 < 224) {
      let c2 = bytes[pos++]
      out[c++] = String.fromCharCode((c1 & 31) << 6 | c2 & 63)
    } else if (c1 > 239 && c1 < 365) {
      // Surrogate Pair
      let c2 = bytes[pos++]
      let c3 = bytes[pos++]
      let c4 = bytes[pos++]
      let u = ((c1 & 7) << 18 | (c2 & 63) << 12 | (c3 & 63) << 6 | c4 & 63) - 0x10000
      out[c++] = String.fromCharCode(0xD800 + (u >> 10))
      out[c++] = String.fromCharCode(0xDC00 + (u & 1023))
    } else {
      let c2 = bytes[pos++]
      let c3 = bytes[pos++]
      out[c++] =
        String.fromCharCode((c1 & 15) << 12 | (c2 & 63) << 6 | c3 & 63)
    }
  }
  return out.join('')
}