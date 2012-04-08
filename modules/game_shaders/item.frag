uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage
uniform sampler2D tex0; // map texture
varying vec2 texCoord; // map texture coords
//uniform int itemId; // item id

void main()
{
    gl_FragColor = texture2D(tex0, texCoord);
}
