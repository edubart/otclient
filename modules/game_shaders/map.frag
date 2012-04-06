uniform float opacity; // painter opacity
uniform vec4 color; // painter color
uniform float time; // time in seconds since shader linkage
uniform sampler2D texture; // map texture
varying vec2 textureCoords; // map texture coords
//uniform vec4 awareArea;

void main()
{
    gl_FragColor = texture2D(texture, textureCoords);
}

/*
vec4 grayScale(vec4 color, float factor)
{
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    return vec4(gray*factor + (1-factor)*color.r,
                gray*factor + (1-factor)*color.g,
                gray*factor + (1-factor)*color.b, 1);
}

// grayscale fog
void main()
{
    vec4 color = texture2D(texture, textureCoords);
    float dist = 0;
    // left
    if(textureCoords.x < awareArea.x && textureCoords.y < awareArea.y)
        dist = distance(textureCoords, awareArea.xy);
    else if(textureCoords.x < awareArea.x && textureCoords.y < awareArea.w)
        dist = distance(textureCoords, vec2(awareArea.x, textureCoords.y));
    else if(textureCoords.x < awareArea.x)
        dist = distance(textureCoords, awareArea.xw);
    // right
    else if(textureCoords.x > awareArea.z && textureCoords.y < awareArea.y)
        dist = distance(textureCoords, awareArea.zy);
    else if(textureCoords.x > awareArea.z && textureCoords.y < awareArea.w)
        dist = distance(textureCoords, vec2(awareArea.z, textureCoords.y));
    else if(textureCoords.x > awareArea.z)
        dist = distance(textureCoords, awareArea.zw);
    // top
    else if(textureCoords.y < awareArea.y)
        dist = distance(textureCoords, vec2(textureCoords.x, awareArea.y));
    // bottom
    else if(textureCoords.y > awareArea.w)
        dist = distance(textureCoords, vec2(textureCoords.x, awareArea.w));
    if(dist > 0) {
        float range = 0.01;
        float factor = min(dist/range, 1.0);
        color = grayScale(color, factor);
        //color.rgb *= 1 - (factor * 0.5);
    }
    gl_FragColor = color;
}
*/

/*
sepia
void main()
{
    vec4 color = texture2D(texture, textureCoords);
    if(textureCoords.x < awareArea.x || textureCoords.y < awareArea.y || textureCoords.x > awareArea.z || textureCoords.y > awareArea.w) {
        gl_FragColor.r = dot(color, vec4(.393, .769, .189, .0));
        gl_FragColor.g = dot(color, vec4(.349, .686, .168, .0));
        gl_FragColor.b = dot(color, vec4(.272, .534, .131, .0));
        gl_FragColor.a = 1;
    } else
        gl_FragColor = color;
}
*/

/*
void main()
{
    vec4 color = texture2D(texture, textureCoords);
    vec2 awareRange = (awareArea.zw - awareArea.xy)/2.0;
    float dist = distance(textureCoords, awareArea.xy + awareRange);
    float start = min(awareRange.x, awareRange.y);
    float range = awareRange*0.1;
    float endFactor = 0.3;
    if(dist >= start) {
        color.rgb *= 1 - (min((dist - start)/range, 1.0) * (2.0 - endFactor));
    }
    gl_FragColor = color;
}
*/