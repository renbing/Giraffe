"\n\
varying mediump vec2 TextureCoordOut;\n\
\n\
uniform lowp float Alpha;\n\
\n\
uniform sampler2D Sampler;\n\
\n\
void main()\n\
{\n\
    gl_FragColor = texture2D(Sampler, TextureCoordOut) * Alpha;\n\
}\n\
";
