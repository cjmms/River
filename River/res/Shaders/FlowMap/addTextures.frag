#version 450 core

// Note: This one relies on blending to work.

// inputs
// ...

// outputs
out vec4 fragColor;

// uniforms
uniform sampler2D uSrc;	// eg. velocity.ping
uniform float uMultiplier;


void main()
{
	vec2 uv = gl_FragCoord.xy;
	fragColor = texture(uSrc, uv)*uMultiplier;
}
