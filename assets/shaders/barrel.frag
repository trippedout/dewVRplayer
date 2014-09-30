#version 120

uniform sampler2DRect tex0;
uniform ivec2 videoSize;
uniform float factor; //amount of distortion

void main()
{
    float xCenter = 0.5f, yCenter = 0.5f; //used for offset
    float blueOffset = 0.0207482; //vals gathered from early testing
    float redOffset = -0.0152678;

    //full size of the eye frame since ARB textures from qtime::getTexture dont use uv 0-1, but total width height
    vec2 fullSize = vec2(960,1080);
    
    vec2 warpCenter = vec2(xCenter, yCenter);
    vec2 arb_fix = gl_TexCoord[0].st / fullSize; //downsample texcoords for 0-1 math
    vec2 centeredTexcoord = arb_fix - warpCenter;
    vec2 warped = normalize(centeredTexcoord);
    
    float rescaled = tan( length( centeredTexcoord ) * factor ) / tan( 0.5f * factor );
    warped *= 0.5f * rescaled;
    
    //rescale back up
    warped *= fullSize;
    warpCenter *= fullSize;
    
    warped += warpCenter;
    
    vec4 result; vec2 sampleLoc;
    
    result = texture2DRect( tex0, vec2(warped.x, fullSize.y - warped.y) );
    sampleLoc = (warped - warpCenter) * blueOffset + warped;
    result.b = texture2DRect( tex0, vec2(sampleLoc.x, fullSize.y - sampleLoc.y) ).b;
    sampleLoc = (warped - warpCenter) * redOffset + warped;
    result.r = texture2DRect( tex0, vec2(sampleLoc.x, fullSize.y - sampleLoc.y) ).r;
    
	gl_FragColor = result;    
}