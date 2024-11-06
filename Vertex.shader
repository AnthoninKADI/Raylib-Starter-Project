#version 330

in vec3 vertexPosition;  
uniform float time;      // Animation Time
uniform mat4 model;      
uniform mat4 view;       
uniform mat4 projection; 

void main()
{
    vec3 pos = vertexPosition;

    pos.y = sin(pos.x * 2.0 + time) * 0.5 + sin(pos.z * 2.0 + time) * 0.5;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}
