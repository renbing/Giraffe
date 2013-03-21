<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="content-type" content="text/html;charset=utf-8">
    <title>Giraffe</title>
<?php
    $jss = split("\n", file_get_contents("index.js"));
    foreach( $jss as $js ) {
        $js = trim($js);
        if( empty($js) ) {
            continue;
        }
        echo "    <script src='$js' type='text/javascript' charset='utf-8'></script>\n";
    }
?>
    <script src='../framework/matrix4x4.js' type='text/javascript' charset='utf-8'></script>
    <script src='../framework/touch.js' type='text/javascript' charset='utf-8'></script>
    <script src='../framework/Render.js' type='text/javascript' charset='utf-8'></script>
    <script src='../framework/MovieClip.js' type='text/javascript' charset='utf-8'></script>
    <script id="shader-fs" type="x-shader/x-fragment">
        precision mediump float;
        varying vec4 vColor;

        void main(void) {
            gl_FragColor = vColor;
        }
    </script>

    <script id="shader-vs" type="x-shader/x-vertex">
        attribute vec4 aVertexPosition;
        attribute vec4 aVertexColor;

        uniform mat4 uMVMatrix;
        uniform mat4 uPMatrix;

        varying vec4 vColor;

        void main(void) {
            gl_Position = uPMatrix * uMVMatrix * aVertexPosition;
            vColor = aVertexColor;
        }
    </script>

    <script id="shader-tfs" type="x-shader/x-fragment">
        precision mediump float;
        varying vec2 vTextureCoord;

        uniform sampler2D uSampler;

        void main(void) {
            gl_FragColor = texture2D(uSampler, vTextureCoord);
        }
    </script>

    <script id="shader-tvs" type="x-shader/x-vertex">
        attribute vec4 aVertexPosition;
        attribute vec2 aTextureCoord;

        uniform mat4 uMVMatrix;
        uniform mat4 uPMatrix;
        uniform mat4 uGLMMatrix;
        uniform vec2 aGLVPosition;

        varying vec2 vTextureCoord;

        void main(void) {
            gl_Position = uPMatrix * (uGLMMatrix * uMVMatrix * aVertexPosition + vec4(aGLVPosition, 0, 0));
            vTextureCoord = aTextureCoord;
        }
    </script>
        
    <script type="text/javascript">
        var trace = function(obj) {console.log(obj);}
        var canvas,gl,glRender;
        var stage = new MovieClip("stage");
        var scaleFactor = 1;

        function webGLStart() {
            canvas = document.getElementById("gl-canvas");
            try{
                gl = canvas.getContext("experimental-webgl");
                gl.viewportWidth = canvas.width;
                gl.viewportHeight = canvas.height;
            }catch(e) {
            }

            if( !gl ) {
                alert("WebGL not support");
                return;
            }

            scaleFactor = canvas.width / parseInt(canvas.style.width);
            Touch.init();

            glRender = new RenderContextGL2();
            main();
            
            var lastTime = +(new Date());
            var counter = 0;
            setInterval(function() {
                var now = +(new Date());
                var passed = now - lastTime;
                lastTime = now;
                counter += 1;

                gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
                mainLoop(passed/1000);

            }, 1000/60);

            setInterval(function() {
                document.getElementById("fps").innerHTML = "" + counter;    
                counter = 0;
            }, 1000);
        }

        ResourceManager.prototype.createTexture = function(image) {
            image.texture = gl.createTexture();
            gl.bindTexture(gl.TEXTURE_2D, image.texture);
            gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);

            image.POTWidth = image.width;
            image.POTHeight = image.height;
        }

        ResourceManager.prototype.mergeImageMask = function(img, mask) {

            var imgCanvas = document.createElement("canvas");
            imgCanvas.width = img.width;
            imgCanvas.height = img.height;
            var imgCtx = imgCanvas.getContext("2d");
            imgCtx.drawImage(img, 0, 0);
            var imgData = imgCtx.getImageData(0, 0, img.width, img.height);

            var maskCanvas = document.createElement("canvas");
            maskCanvas.width = mask.width;
            maskCanvas.height = mask.height;
            var maskCtx = maskCanvas.getContext("2d");
            maskCtx.drawImage(mask, 0, 0);
            var maskData = maskCtx.getImageData(0, 0, mask.width, mask.height);
            
            for( var y=0, maxY=imgData.height; y<maxY; y++ ) {
                for( var x=0, maxX=imgData.width; x<maxX; x++ ) {
                    var r = maskData.data[(x + y * maxX) * 4];
                    imgData.data[(x + y * maxX) * 4 + 3] = r;
                }
            }
            imgCtx.putImageData(imgData, 0, 0);

            return imgCanvas;
        };

    </script>
</head>
<body style="margin:0;padding:0" onload="webGLStart()">
    <div style="float:left">FPS:</div><div id="fps" style="float:left">60</div>
    <canvas id="gl-canvas" style="border:none;width:440px;padding-left:20px;" width=640 height=960></canvas>
</body>
</html>
