"\n\
attribute vec4 Position;\n\
attribute vec2 TextureCoord;\n\
\n\
uniform mat4 TSRMatrix;\n\
\n\
varying vec2 TextureCoordOut;\n\
\n\
uniform mat4 Projection;\n\
\n\
void main()\n\
{\n\
    gl_Position = Projection * TSRMatrix * Position;\n\
    TextureCoordOut = TextureCoord;\n\
}\n\
";
