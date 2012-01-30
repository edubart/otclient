uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage
uniform sampler2D texture; // map texture
varying vec2 textureCoords; // map texture coords

void main()
{
    vec4 outColor = texture2D(texture, textureCoords) * opacity;
    /*
    float refinement = 0;
    if(refinement > 0) {
        vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);
        for(int i=-1;i<=1;++i) {
            for(int j=-1;j<=1;++j) {
                vec4 pixel = texture2D(texture, textureCoords + vec2(i,j)*(1.0/32.0));
                sum += pixel * (1.0/9.0) * pixel.a * (0.5 + (pixel.r + pixel.g + pixel.b)/3.0);
            }
        }
        float factor = refinement*((1.0 + sin(time*2))/2.0);
        outColor += sum * sum * factor;
    }
    */
    gl_FragColor = outColor;
}
