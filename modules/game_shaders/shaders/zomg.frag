uniform sampler2D u_Tex0;
uniform float u_Time;
varying vec2 v_TexCoord;

vec2 tibiaDir = vec2(1.0, 1.0);
void main(void)
{
    vec2 dir = 0.5 - v_TexCoord;
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y);
    float scale = 0.8 + dist*0.5;
    vec4 color = texture2D(u_Tex0, -(dir*scale - 0.5));
    gl_FragColor = color;
}
