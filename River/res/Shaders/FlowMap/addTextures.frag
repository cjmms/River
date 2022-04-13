#version 450 core

// Note: This one relies on blending to work.

// inputs
in vec2 TexCoords;

// outputs
out vec4 fragColor;

// uniforms
uniform sampler2D uSrc;	// eg. velocity.ping
uniform float uMultiplier;
uniform vec2 uDirection;

uniform vec2 uDstScale = vec2(1024);

void main()
{
	vec2 uv = gl_FragCoord.xy/uDstScale;
	//fragColor = texture(uSrc, uv)*uMultiplier;
	
	const vec2 normalVector = vec2(-uDirection.y, uDirection.x);
	const vec2 tex = texture(uSrc, uv).xy*uMultiplier;
	
	fragColor = vec4(vec2(tex.x*uDirection) + (tex.y*normalVector), 0, 1);

	// Updated to a version which maps the color range (0, 1) to (-1, 1)
	//fragColor = (texture(uSrc, uv)-vec4(0.5f))*2.0f*uMultiplier;


	// Fine-tuned in shader code to get a particular bias. Bad practice, but i just want it to be done.
	//fragColor = (texture(uSrc, uv)-vec4(0.1f, 0.5, 0, 0))*uMultiplier;

	//fragColor = vec4(TexCoords,0, 1);
}
