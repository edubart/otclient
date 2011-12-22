uniform float opacity;
uniform vec4 color;
uniform float ticks;

uniform sampler2D texture; // outfit texture
varying vec2 textureCoords; // outfit texture coords
uniform sampler2D maskTexture; // outfit color mask

uniform vec4 headColor;
uniform vec4 bodyColor;
uniform vec4 legsColor;
uniform vec4 feetColor;

const vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
const vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);

vec4 calcOutfitPixel()
{
    vec4 pixel = texture2D(texture, textureCoords);
    vec4 maskColor = texture2D(maskTexture, textureCoords);

    vec4 outColor = vec4(1.0, 1.0, 1.0, 1.0);
    if(maskColor == yellow)
        outColor = headColor;
    else if(maskColor == red)
        outColor = bodyColor;
    else if(maskColor == green)
        outColor = legsColor;
    else if(maskColor == blue)
        outColor = feetColor;

    return pixel * outColor;
}

void main()
{
    gl_FragColor = calcOutfitPixel() * color * opacity;
}
