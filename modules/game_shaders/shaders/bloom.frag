uniform float u_Time;
uniform sampler2D u_Tex0;
varying vec2 v_TexCoord;

void main()
{
    vec4 color = texture2D(u_Tex0, v_TexCoord);
    int j;
    int i;

    for(i = -4 ;i <= 4; i++)
        for(j = -4; j <= 4; j++)
            color += texture2D(u_Tex0, v_TexCoord + vec2(i, j)*0.003) * 0.008;

    gl_FragColor = color;
}
