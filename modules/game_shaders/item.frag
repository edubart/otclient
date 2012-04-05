uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage
uniform sampler2D texture; // map texture
//uniform int itemId; // item id
varying vec2 textureCoords; // map texture coords

void main()
{
    gl_FragColor = texture2D(texture, textureCoords);
}
