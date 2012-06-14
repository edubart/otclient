uniform float u_Time;
uniform sampler2D u_Tex0;
varying vec2 v_TexCoord;

void main()
{
    gl_FragColor = texture2D(u_Tex0, v_TexCoord);
}
