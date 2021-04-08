#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 initial_color;

out vec4 fragColor;

uniform mat4 Projection;
uniform mat4 Model;

void main(){
	gl_Position = Projection * Model * vec4(aPos, 1.0);
	fragColor = initial_color;
}