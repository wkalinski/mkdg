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

    bool isBorder= false;
    if(gl_FragCoord.x < borderWidth || 
    gl_FragCoord.y < borderWidth || 
    textureWidth - gl_FragCoord.x < borderWidth ||
    textureHeight - gl_FragCoord.y < borderWidth)
    {
        isBorder = true;
    }

    if(isBorder)
    {
        FragColor = vec4(1.0, 0.0, 0.6, 1.0);
    }
    else
    {   
        float N = (windowSize* 2 + 1) * (windowSize* 2 + 1);
        float sqrtN = (windowSize* 2 + 1);

        vec3 average = vec3(0.0, 0.0, 0.0);

        for(int dx = - windowSize; dx <= windowSize; dx++)
        {
           for(int dy = - windowSize; dy <= windowSize; dy++)
            {
                vec3 partialColor = sampleTexture(tex, texCoordi.x + dx, texCoordi.y + dy).rgb;
                average += partialColor / N;
            }
        }
        FragColor = vec4(average, 1.0);
    }
}