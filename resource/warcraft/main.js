
function mainLoop(passed) {
    stage.render();
    Tween.update(passed);
    Timer.update(passed);
}

function main() {
    trace("main");
    resourceManager.add("cubetexture.png", "image");
    resourceManager.load(start);
}

function start() {

    audio = new Audio();
    audio.canplay = function(){trace('audio canplay')};
    audio.src = "home_music.mp3";

    var image = resourceManager.get("cubetexture.png");
    var bitmap = new Bitmap(image, "bitmap");
    bitmap.y = 100;
    bitmap.addEventListener(Event.TAP, function(e) {
        trace('bitmap taped');
    });
    stage.addChild(bitmap);
    
    Tween.move(bitmap, Tween.BACK_EASE_IN, 1, 300, 100, 1)
        .seqMove(bitmap, Tween.BACK_EASE_IN, 1, 0, 100, 0);

    Timer.setTimeout(function(){
        //stage.removeChild(bitmap);
    }, 3);
    
    Ajax.get("http://192.168.1.111/a.php", function(status, url, xhr) {
        trace('ajax get finished:' + status);
    });
}
