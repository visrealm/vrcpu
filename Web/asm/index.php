<!DOCTYPE html>


<html>

<head>
  <title>Troy's Breadboard Computer - Assembler</title>
  <meta charset="utf-8">
  <link rel="shortcut icon" href="https://visualrealmsoftware.com/sites/default/files/favicon2.png" type="image/png" />
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css"
    integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous" />
  <link rel="stylesheet" href="codemirror/codemirror.css" />
  <link rel="stylesheet" href="codemirror/cm_dark.css" />
</head>


<style>
body {
  background-image: url("../img/computer1.jpg");
  background-repeat: no-repeat;
  background-size: cover;
}

html,
body {
  margin: 0;
  border: 0;
  padding: 0;
  width: 100%;
  height: 100%;
  font-family: Segoe UI, Verdana, sans-serif;
  overflow: hidden;
  background-color: #eee;
}

#divGrid {
  margin: 0.5em;
  width: calc(100% - 1em);
  height: calc(100% - 6em);
  display: grid;
  grid-template-columns: 1fr 1fr;
  grid-template-rows: auto 1fr;
  grid-gap: 10px;
  grid-template-areas:
    "h h"
    "i o"
}

#divInput {
  border: 5px solid #ccc;
  border-radius: 5px;
  width: calc(100% - 20px);
  height: calc(100% - 20px);
  grid-area: i;
}

#divCodeMirror {
  width: 100%;
  height: 100%;
  font-size: 1.3em;
}

#divOutput {
  border: 5px solid #ccc;
  border-radius: 5px;
  padding: -5px;
  width: calc(100% - 20px);
  height: calc(100% - 20px);
  grid-area: o;
  background-color: #262626f0;
  color: #EBEFE7;
  overflow: auto;
}

#divOutputText {
  width: 100px;
  height: 100px;
  font-family: Consolas, monospace;
  white-space: pre;
}

span::selection {
  background: rgba(69, 68, 59, .90);
}

#divHeader {
  grid-area: h;
  align-self: center;
  justify-self: center;
}
</style>

<script src="https://code.jquery.com/jquery-3.3.1.min.js" crossorigin="anonymous"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js"
  integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js"
  integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>

<body onload="main()">
  <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
    <a class="navbar-brand" href="https://cpu.visualrealmsoftware.com">Troy's Breadboard Computer</a>
    <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNav"
      aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
      <span class="navbar-toggler-icon"></span>
    </button>
    <div class="collapse navbar-collapse" id="navbarNav">
      <ul class="navbar-nav">
        <li class="nav-item active">
          <a class="nav-link" href="https://cpu.visualrealmsoftware.com/asm">Assembler</a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="https://cpu.visualrealmsoftware.com/emu">Emulator</a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="https://cpu.visualrealmsoftware.com/docs">Documentation</a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="https://cpu.visualrealmsoftware.com/gallery">Gallery</a>
        </li>
      </ul>

    </div>
    <div class="navbar-collapse  w-75 order-3 dual-collapse2">

      <ul class="navbar-nav ml-auto">
        <li class="nav-item">
          <div class="dropdown" style="margin-right:10px;">
            <button class="btn btn-success dropdown-toggle" type="button" id="loadDropdown" data-toggle="dropdown"
              aria-haspopup="true" aria-expanded="false">
              Examples
            </button>
            <div class="dropdown-menu" aria-labelledby="dropdownMenuButton">
              <?
        $files = glob('examples/*.{asm}', GLOB_BRACE);
        foreach($files as $file) {
?>
              <a class="dropdown-item" href="#"
                onclick="loadExample('<?=$file?>')"><?=str_replace(array("examples/", ".asm"), "", $file)?></a>
              <?
        }
?>
            </div>
          </div>
        </li>
        <li class="nav-item">

          <button type="button" class="btn btn-primary my-2 my-sm-0" id="buttonAssemble" onclick="assemble()"
            data-toggle="tooltip" title="Attempt to assemble your program">Assemble (Ctrl+Enter) &gt;&gt;</button>
          <button type="button" class="btn btn-secondary my-2 my-sm-0" id="buttonEmulate" class="buttonEmulate"
            onclick="emulate()" data-toggle="tooltip" title="Run this program on the Emulator."
            disabled>Emulate</button>
          <button type="button" class="btn btn-secondary my-2 my-sm-0" id="buttonSubmit" class="buttonSubmit"
            onclick="submit()" data-toggle="tooltip" title="Run on the real deal." disabled>Submit</button>
        </li>
      </ul>
    </div>
  </nav>

  <div id="divGrid">
    <div id="divInput">
      <div id="divCodeMirror"></div>
    </div>
    <div id="divHeader">
    </div>
    <div id="divOutput">
      <div id="divOutputText"></div>
    </div>
    <nav class="navbar fixed-bottom navbar-expand-sm navbar-dark bg-dark"><span  class="text-white">Assembler powered by <a href="https://github.com/hlorenzi/customasm">customasm</a></span></nav>
  </div>

  <script src="underscore.min.js"></script>
  <script src="main.js"></script>
  <script src="codemirror/codemirror.js"></script>
  <script src="codemirror/troy_syntax.js"></script>
</body>

</html>