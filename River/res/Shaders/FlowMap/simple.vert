#version 450 core
layout(Location = 0) in vec2 aPos;
layout(Location = 1) in vec2 aTextureCoord;

out vec2 TexCoords;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
    TexCoords = aTextureCoord;
}

