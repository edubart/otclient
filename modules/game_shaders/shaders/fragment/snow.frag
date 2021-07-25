uniform float u_Time;
uniform sampler2D u_Tex0; // game screen
uniform sampler2D u_Tex1; // snow
varying vec2 v_TexCoord;
uniform vec2 u_WalkOffset;

vec2 snowDirection = vec2(0.5, 1.0);
float snowSpeed = 0.08;
float snowPressure = 0.4;
float snowZoom = 0.1;

void main(void)
{
  vec3 Game = texture2D(u_Tex0, v_TexCoord).xyz;
  vec2 SnowHandler = (v_TexCoord + vec2(u_WalkOffset.x, u_WalkOffset.y) + (snowDirection * u_Time * snowSpeed)) / snowZoom;
  vec3 Snow = texture2D(u_Tex1, SnowHandler).xyz;

  vec3 _output = Game + Snow * snowPressure;
  gl_FragColor = vec4(_output, 1.0);
}
