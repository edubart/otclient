uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage
uniform sampler2D texture; // map texture
varying vec2 textureCoords; // map texture coords
//uniform int itemId; // item id

void main()
{
    vec4 outColor = texture2D(texture, textureCoords);
    gl_FragColor = outColor * opacity;
}
