#version 120

uniform sampler2DRect tex0;
uniform ivec2 videoSize;
uniform bool isLeftEye;

void main()
{
    float x = 1.0f - gl_TexCoord[0].s; //flip x
    float y = gl_TexCoord[0].t;
    int halfVideo = videoSize.y/2;
    vec4 result;
    
    if(isLeftEye)
        result = texture2DRect( tex0, vec2( x * videoSize.x, y * videoSize.y/2) );
    else
        result = texture2DRect( tex0, vec2( x * videoSize.x, halfVideo + y * videoSize.y/2) );
    
	gl_FragColor = result;
}