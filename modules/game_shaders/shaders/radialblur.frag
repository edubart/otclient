uniform sampler2D u_Tex0;
varying vec2 v_TexCoord;

// some const, tweak for best look
const float sampleDist = 1.0;
const float sampleStrength = 2.2;

void main(void)
{
    // 0.5,0.5 is the center of the screen
    // so substracting v_TexCoord from it will result in
    // a vector pointing to the middle of the screen
    vec2 dir = 0.5 - v_TexCoord;

    // calculate the distance to the center of the screen
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y);

    // normalize the direction (reuse the distance)
    dir = dir/dist;

    // this is the original colour of this fragment
    // using only this would result in a nonblurred version
    vec4 color = texture2D(u_Tex0,v_TexCoord);

    vec4 sum = color;

    // take 10 additional blur samples in the direction towards
    // the center of the screen
    sum += texture2D(u_Tex0, v_TexCoord - 0.08 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord - 0.05 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord - 0.03 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord - 0.02 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord - 0.01 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord + 0.01 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord + 0.02 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord + 0.03 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord + 0.05 * dir * sampleDist);
    sum += texture2D(u_Tex0, v_TexCoord + 0.08 * dir * sampleDist);

    // we have taken eleven samples
    sum *= 1.0/11.0;

    // weighten the blur effect with the distance to the
    // center of the screen ( further out is blurred more)
    float t = dist * sampleStrength;
    t = clamp(t ,0.0,1.0); //0 &lt;= t &lt;= 1

    //Blend the original color with the averaged pixels
    gl_FragColor = mix(color, sum, t);
}