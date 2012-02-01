uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage
uniform sampler2D texture; // map texture
varying vec2 textureCoords; // map texture coords

void main()
{
    vec4 outColor = texture2D(texture, textureCoords) * opacity;
    gl_FragColor = outColor;
}
