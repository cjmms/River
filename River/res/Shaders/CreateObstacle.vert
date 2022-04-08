#version 450 core
layout (location = 0) in vec3 aPos;


uniform mat4 inverseModel;

void main()
{
	// input pos is the world space intersection between ray(mouse clicking) and plane(river)
	// 1. apply inverse of plane model matrix, transfrom vertices into [-1, 1], local space of plane. 
	vec4 localSpacePos = inverseModel * vec4(aPos, 1);

	// 2. transform pos from plane local space [-1, 1] into screen space [0, 1]
	vec2 screenSpace = localSpacePos.xy;

	gl_Position = vec4(screenSpace.x, -screenSpace.y, 0.0, 1);
}
