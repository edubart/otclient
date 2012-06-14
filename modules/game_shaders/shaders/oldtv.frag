uniform float u_Time;
varying vec2 v_TexCoord;
uniform sampler2D u_Tex0;

void main(void)
{
    vec2 q = v_TexCoord;
    vec2 uv = 0.5 + (q-0.5)*(0.9 + 0.1*sin(0.2*u_Time));

    vec3 oricol = texture2D(u_Tex0,vec2(q.x,q.y)).xyz;
    vec3 col = oricol;

    col = clamp(col*0.5+0.5*col*col*1.2,0.0,1.0);

    col *= 0.5 + 0.5*16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y);

    col *= vec3(0.8,1.0,0.7);

    col *= 0.9+0.1*sin(10.0*u_Time+uv.y*1000.0);

    col *= 0.97+0.03*sin(110.0*u_Time);

    gl_FragColor = vec4(col,1.0);
}
