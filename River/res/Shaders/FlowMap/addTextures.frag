#version 450 core

// Note: This one relies on blending to work.

// inputs
in vec2 TexCoords;

// outputs
out vec4 fragColor;

// uniforms
uniform sampler2D uSrc;	// eg. velocity.ping
uniform float uMultiplier;

uniform vec2 uDstScale = vec2(512);

void main()
{
	vec2 uv = gl_FragCoord.xy/uDstScale;
	//fragColor = texture(uSrc, uv)*uMultiplier;
	
	// Updated to a version which maps the color range (0, 1) to (-1, 1)
	fragColor = (texture(uSrc, uv)-vec4(0.5f))*2.0f*uMultiplier;
	//fragColor = vec4(TexCoords,0, 1);
}
