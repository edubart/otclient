uniform float u_Time;
uniform sampler2D u_Tex0;
uniform sampler2D u_Tex1;
varying vec2 v_TexCoord;
uniform vec2 u_WalkOffset;

vec2 direction = vec2(1.0, 0.3);
float speed = 0.05;
float pressure = 0.6;
float zoom = 0.5;

void main(void)
{
  vec2 test = v_TexCoord + vec2(u_WalkOffset.x, u_WalkOffset.y) + (direction * u_Time * speed);
  vec3 bgcol = texture2D(u_Tex0, v_TexCoord).xyz;
  vec3 fogcol = texture2D(u_Tex1, test).xyz;
  vec3 col = bgcol + fogcol * pressure;
  gl_FragColor = vec4(col, 1.0);
}
