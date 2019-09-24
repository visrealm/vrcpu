<!DOCTYPE html>
<html>

<head>
  <title>Troy's Breadboard Computer - Gallery</title>
  <link rel="shortcut icon" href="https://visualrealmsoftware.com/sites/default/files/favicon2.png" type="image/png" />
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css"
    integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.0.13/css/all.css"
    integrity="sha384-DNOHZ68U8hZfKXOrtjWvjxusGo9WQnrNx2sqG0tfsghAvtVlRW3tvkXWZh58N9jp" crossorigin="anonymous">
  <style>
  body {
    background-image: url("../img/computer1.jpg");
    background-repeat: no-repeat;
    background-size: cover;
  }

  .img-thumbnail {
    margin-top: 10px;
    margin-bottom: 10px;
  }
  </style>


</head>
<script src="https://code.jquery.com/jquery-3.3.1.slim.min.js"
  integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js"
  integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js"
  integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>

<body>

  <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
    <a class="navbar-brand" href="https://cpu.visualrealmsoftware.com">Troy's Breadboard Computer</a>
    <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNav"
      aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
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
        <li class="nav-item">
          <a class="nav-link" href="https://cpu.visualrealmsoftware.com/docs">Documentation</a>
        </li>
        <li class="nav-item active">
          <a class="nav-link" href="https://cpu.visualrealmsoftware.com/gallery">Gallery</a>
        </li>
      </ul>
    </div>
  </nav>
  <div class="container2" style="background: #262626f0;
    width: 100%;
    height: 100%;
    padding: 20px;">
    <div class="row">

      <?	    
$files = glob('img/*.{jpg,mp4}', GLOB_BRACE);
foreach($files as $file) {
if (strpos($file, "mp4") === false)
{?>
      <div class="col-lg-3 col-md-4 col-xs-6 thumb">
        <a class="thumbnail" href="#" data-image-id="" data-toggle="modal"
          data-title="<?=file_exists($file.".txt") ? file_get_contents($file.".txt") : str_replace(array("img/",".jpg"), "",$file)?>"
          data-image="<?=$file?>" data-target="#image-gallery">
          <img class="img-thumbnail" src="<?=$file?>" alt="Another alt text">
        </a>
      </div>
      <?
}
else
{
?>
      <div class="col-lg-3 col-md-4 col-xs-6 thumb">
        <video width="100%" height="100%" controls class="img-thumbnail">
          <source src="<?=$file?>" type="video/mp4"></video>
      </div>
      <?
}
}
?>
    </div>

    <div class="modal fade" id="image-gallery" tabindex="-1" role="dialog" aria-labelledby="myModalLabel"
      aria-hidden="true">
      <div class="modal-dialog modal-lg">
        <div class="modal-content">
          <div class="modal-header">
            <h4 class="modal-title" id="image-gallery-title"></h4>
            <button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">×</span><span
                class="sr-only">Close</span>
            </button>
          </div>
          <div class="modal-body">
            <img id="image-gallery-image" class="img-responsive col-md-12" src="">
          </div>
          <div class="modal-footer">
            <button type="button" class="btn btn-secondary float-left" id="show-previous-image"><i
                class="fa fa-arrow-left"></i>
            </button>

            <button type="button" id="show-next-image" class="btn btn-secondary float-right"><i
                class="fa fa-arrow-right"></i>
            </button>
          </div>
        </div>
      </div>
    </div>
</body>
<script>
let modalId = $('#image-gallery');

$(document)
  .ready(function() {

    loadGallery(true, 'a.thumbnail');

    //This function disables buttons when needed
    function disableButtons(counter_max, counter_current) {
      $('#show-previous-image, #show-next-image')
        .show();
      if (counter_max === counter_current) {
        $('#show-next-image')
          .hide();
      } else if (counter_current === 1) {
        $('#show-previous-image')
          .hide();
      }
    }

    /**
     *
     * @param setIDs        Sets IDs when DOM is loaded. If using a PHP counter, set to false.
     * @param setClickAttr  Sets the attribute for the click handler.
     */

    function loadGallery(setIDs, setClickAttr) {
      let current_image,
        selector,
        counter = 0;

      $('#show-next-image, #show-previous-image')
        .click(function() {
          if ($(this)
            .attr('id') === 'show-previous-image') {
            current_image--;
          } else {
            current_image++;
          }

          selector = $('[data-image-id="' + current_image + '"]');
          updateGallery(selector);
        });

      function updateGallery(selector) {
        let $sel = selector;
        current_image = $sel.data('image-id');
        $('#image-gallery-title')
          .text($sel.data('title'));
        $('#image-gallery-image')
          .attr('src', $sel.data('image'));
        disableButtons(counter, $sel.data('image-id'));
      }

      if (setIDs == true) {
        $('[data-image-id]')
          .each(function() {
            counter++;
            $(this)
              .attr('data-image-id', counter);
          });
      }
      $(setClickAttr)
        .on('click', function() {
          updateGallery($(this));
        });
    }
  });

// build key actions
$(document)
  .keydown(function(e) {
    switch (e.which) {
      case 37: // left
        if ((modalId.data('bs.modal') || {})._isShown && $('#show-previous-image').is(":visible")) {
          $('#show-previous-image')
            .click();
        }
        break;

      case 39: // right
        if ((modalId.data('bs.modal') || {})._isShown && $('#show-next-image').is(":visible")) {
          $('#show-next-image')
            .click();
        }
        break;

      default:
        return; // exit this handler for other keys
    }
    e.preventDefault(); // prevent the default action (scroll / move caret)
  });
</script>

</html>