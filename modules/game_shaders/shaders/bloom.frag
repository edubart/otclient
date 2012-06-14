uniform float u_Time;
uniform sampler2D u_Tex0;
varying vec2 v_TexCoord;

void main()
{
    vec4 color = texture2D(u_Tex0, v_TexCoord);
    int j;
    int i;

    for(i = -2 ;i <= 2; i++)
        for(j = -2; j <= 2; j++)
            color += texture2D(u_Tex0, v_TexCoord + vec2(i, j)*0.0025) * 0.025;

    gl_FragColor = color;
}
