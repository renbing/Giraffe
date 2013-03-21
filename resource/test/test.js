var p = new Point();
trace('hello native trace');
trace(Point.call());
trace(p.x);
p.x = 1;
p.y = 2;
trace(p.y);
trace(p.x);
trace(Point.NAME);
trace(p.callback);
p.callback=function(){
    trace('point callback called');
};
trace(p.callback);
p.talk();

function main() {

    var img = new Image();
    img.onload = function() {
        trace("image onload");
    };
    img.src = "../../cubetexture.png";
//    p.callback = img;
//    p.callback = null;
}

function mainLoop() {
    //p.x = 10;
}