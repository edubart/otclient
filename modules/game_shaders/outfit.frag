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

/*
// optimized to handle antialising
vec4 calcOutfitPixel()
{
    vec4 pixel = texture2D(texture, textureCoords);
    vec4 maskColor = texture2D(maskTexture, textureCoords);
    const vec4 white = vec4(1,1,1,1);

    float headFactor = 0.0;
    if(maskColor.r > 0.1 && maskColor.g > 0.1) {
        headFactor = min(maskColor.r, maskColor.g);

        maskColor.r -= headFactor;
        maskColor.g -= headFactor;
    }

    float tot = headFactor + maskColor.r + maskColor.g + maskColor.b;
    vec4 outColor = headFactor * headColor + bodyColor * maskColor.r + legsColor * maskColor.g + feetColor * maskColor.b;
    if(tot < 1.0)
        outColor += white * (1.0 - tot);
    outColor.a = 1.0;

    return pixel * outColor;
}
*/
void main()
{
    gl_FragColor = calcOutfitPixel();
}

