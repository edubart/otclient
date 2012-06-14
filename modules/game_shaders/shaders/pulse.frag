uniform float u_Time;
uniform vec2 u_Resolution;
uniform sampler2D u_Tex0;
varying vec2 v_TexCoord;

void main(void)
{
    vec2 halfres = u_Resolution.xy/2.0;
    vec2 cPos = v_TexCoord.xy * u_Resolution;

    cPos.x -= 0.5*halfres.x*sin(u_Time/2.0)+0.3*halfres.x*cos(u_Time)+halfres.x;
    cPos.y -= 0.4*halfres.y*sin(u_Time/5.0)+0.3*halfres.y*cos(u_Time)+halfres.y;
    float cLength = length(cPos);

    vec2 uv = v_TexCoord.xy+ ((cPos/cLength)*sin(cLength/30.0-u_Time*10.0)/25.0)*0.15;
    vec3 col = texture2D(u_Tex0,uv).xyz * 250.0/cLength;

    gl_FragColor = vec4(col,1.0);
}