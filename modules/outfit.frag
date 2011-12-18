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

vec4 calcPixel(vec2 texCoord)
{
    vec4 pixel = texture2D(texture, texCoord);
    vec4 maskColor = texture2D(maskTexture, texCoord);

    vec4 outColor = vec4(0);
    if(maskColor.r == 1.0 && maskColor.g == 1.0) {
        outColor = headColor;
    } else if(maskColor.r == 1.0) {
        outColor = bodyColor;
    } else if(maskColor.g == 1.0) {
        outColor = legsColor;
    } else if(maskColor.b == 1.0) {
        outColor = feetColor;
    }

    if(outColor.a != 0.0)
        pixel = pixel * outColor;
    return pixel;
}
void main()
{
    vec4 pixel = calcPixel(textureCoords);
    int num = 16;

    vec4 sum = vec4(0);
    int i, j;
    for(i=-num/2;i<num/2;++i) {
        for(j=-num/2;j<num/2;++j) {
            sum += calcPixel(textureCoords + vec2(i+1, j+1)*0.005) * 1.0/(num*num);
        }
    }
    sum = sin(ticks/500.0)*sum;

    gl_FragColor = pixel * color * opacity;
}
