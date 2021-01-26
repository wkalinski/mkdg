#version 330 core
out vec4 FragColor;
  
uniform int textureWidth;
uniform int textureHeight;

uniform sampler2D tex;

uniform int localWindowSize;

vec4 sampleTexture(in sampler2D tex,in int x,in int y)
{
    vec2 texCoord = vec2(float(x) / textureWidth, 1.0 - ( float(y) / textureHeight) );
    vec4 ret = texture(tex, texCoord);
    return (ret);
}

vec3 toGray(in vec3 color)
{
    float avg = (color.r + color.g + color.b) / 3.0;
    return vec3(avg, avg, avg);
}

void main()
{
    ivec2 texCoordi = ivec2(gl_FragCoord.x, gl_FragCoord.y);

    int windowSize = localWindowSize;

    int borderWidth = windowSize - 1;

   
    vec3 rgb = sampleTexture(tex, texCoordi.x, texCoordi.y).rgb;
    gl_FragColor = vec4(rgb, 1.0);

}