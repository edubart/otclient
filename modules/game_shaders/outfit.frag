uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage

uniform sampler2D texture; // outfit texture
varying vec2 textureCoords; // outfit texture coords
uniform sampler2D maskTexture; // outfit color mask

uniform vec4 headColor;
uniform vec4 bodyColor;
uniform vec4 legsColor;
uniform vec4 feetColor;

vec4 calcOutfitPixel()
{
    vec4 pixel = texture2D(texture, textureCoords);
    vec4 maskColor = texture2D(maskTexture, textureCoords);

    vec4 outColor = vec4(1.0, 1.0, 1.0, 1.0);
    if(maskColor.r > 0.1 && maskColor.g > 0.1)
        outColor = headColor;
    else if(maskColor.r > 0.1)
        outColor = bodyColor;
    else if(maskColor.g > 0.1)
        outColor = legsColor;
    else if(maskColor.b > 0.1)
        outColor = feetColor;

    return pixel * outColor;
}

void main()
{
    gl_FragColor = calcOutfitPixel() * color * opacity;
}

