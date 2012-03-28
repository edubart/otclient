uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage
uniform sampler2D texture; // map texture
varying vec2 textureCoords; // map texture coords

/*
void main()
{
    gl_FragColor = texture2D(texture, textureCoords) * color * opacity;
}
*/
void main()
{
   vec4 sum = vec4(0);
   vec2 texcoord = textureCoords;
   int j;
   int i;

   for( i= -4 ;i < 4; i++)
   {
        for (j = -4; j < 4; j++)
        {
            sum += texture2D(texture, texcoord + vec2(j, i)*0.0005) * 0.008;
        }
   }
   gl_FragColor = texture2D(texture, textureCoords) * color * opacity + sum;
}
