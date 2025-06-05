#<fragment shader>

#version 460 core 

layout (location = 0) out vec4 frag_color;

in vec3 v_position;

uniform vec4 color;

void main(){
	bool border = any(greaterThanEqual(abs(v_position.xy), vec2(0.49)));
	frag_color = border ? vec4(0, 0, 0, 1) : color;
 }

