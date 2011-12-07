uniform sampler2D texture;
varying vec2 textureCoords;
uniform vec4 color;
uniform float opacity;

uniform float ticks;
uniform float rt_w = 18*32;
uniform float rt_h = 14*32;

uniform float radius = 300.0;
uniform float angle = 0.2;
uniform vec2 center = vec2(8*32, 5*32);

vec4 PostFX(sampler2D tex, vec2 uv, float time)
{
  vec2 texSize = vec2(rt_w, rt_h);
  vec2 tc = uv * texSize;
  tc -= center;
  float dist = length(tc);
  if (dist < radius)
  {
    float percent = (radius - dist) / radius;
    float theta = percent * percent * ((int)ticks % 1000)/1000.0 * 8.0;
    float s = sin(theta);
    float c = cos(theta);
    tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
  }
  tc += center;
  vec3 color = texture2D(texture, tc / texSize).rgb;
  return vec4(color, 1.0);
}

void main (void)
{
  vec2 uv = textureCoords.st;
  gl_FragColor = PostFX(texture, uv, ticks) * opacity;
}

/*
uniform float opacity;
vec4 calculatePixel();
void main()
{
    gl_FragColor = calculatePixel() * opacity;
}

varying vec2 textureCoords;
uniform vec4 color;
uniform sampler2D texture;
vec4 calculatePixel() {
    return texture2D(texture, textureCoords) * color;
}
*/