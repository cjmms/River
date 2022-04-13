#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aDir;
layout (location = 2) in float aHeight;
layout (location = 3) in float aSpeed;
layout (location = 4) in float aRadius;

out vec2 velocity;
out float amplitude;

uniform float time;
uniform float timeScale;
uniform float heightScale;

void main()
{

	vec2 direction = normalize(aDir + vec2(10.0, 0.0f)); // <-- hacky way to do this!
	//vec3 velocity = texture(flowMap, );
	vec2 pos = aPos + aSpeed * float(time) * float(timeScale) * direction;	// update position base on wave particle velocity

	vec2 newPos = abs(pos);

	// check if new position is outside the screen
	if (newPos.x > 1 || newPos.y > 1)
	{
		vec2 offset = vec2(0, 0);

		// calculate the decimal part of position
		vec2 posI = vec2( int(newPos.x), int(newPos.y));	// the integer part
		vec2 posF = newPos - posI;						// new pos - integer position

		if (newPos.x > 1.0)
        {
            offset.x = int(posI.x - 1) % 2 + posF.x;
            pos.x = sign(pos.x) * offset.x + sign(pos.x) * -1;
        }
    
        if (newPos.y > 1.0)
        {
            offset.y = int(posI.y - 1) % 2 + posF.y;
            pos.y = sign(pos.y) * offset.y + sign(pos.y) * -1;
        }
	}



	gl_Position = vec4(pos, 0.1, 1);

	velocity = direction * aSpeed;
	amplitude = aHeight * heightScale;
}
