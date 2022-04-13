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
	fragColor = texture(uSrc, uv)*uMultiplier;
	//fragColor = vec4(TexCoords,0, 1);
}
