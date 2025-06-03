#<fragment shader>

#version 460 core 

layout (location = 0) out vec4 frag_color;

in vec3 v_position;

uniform vec4 value;

void main(){
	frag_color = value;
}

