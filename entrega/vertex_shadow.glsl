#version 410

layout(location = 0) in vec4 coord;
layout(location = 1) in vec3 normal;

uniform mat4 Mv;  
uniform mat4 Mn;  
uniform mat4 Mvp; 
uniform vec4 lpos;  
out vec3 veye;      
out vec3 neye;      
out vec3 lightDir;  
out vec4 llpos;

void main(void) 
{
    veye = vec3(Mv * coord);
        
    if (lpos.w == 0.0) 
        lightDir = normalize(vec3(lpos));  
    else
        lightDir = normalize(vec3(lpos) - veye);  
    
    neye = normalize(vec3(Mn * vec4(normal, 0.0f)));
    
    gl_Position = Mvp * coord;
    llpos = lpos;
}
