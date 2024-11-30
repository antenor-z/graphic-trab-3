#version 410

in vec3 veye;      
in vec3 neye;      
in vec3 lightDir;  
in vec4 llpos;

uniform vec4 lamb; 
uniform vec4 ldif; 
uniform vec4 lspe; 

uniform vec4 mamb; 
uniform vec4 mdif; 
uniform vec4 mspe; 
uniform float mshi; 

uniform bool amb_only;

out vec4 fcolor; 

void main(void)
{
    vec3 n = normalize(neye);
    vec3 l = normalize(lightDir);
    
    vec4 ambient = mamb * lamb;
    
    float ndotl = dot(n, l);
    vec4 diffuse = mdif * ldif * ndotl;
    vec4 specular = vec4(0.0);
    if (ndotl > 0.0) {
        vec3 viewDir = normalize(vec3(llpos) - veye); 
        vec3 reflectDir = reflect(-l, n); 
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), mshi);
        specular = mspe * lspe * spec;
    }
    
    if (amb_only) {
        fcolor = ambient;
    }
    else {
        fcolor = ambient + diffuse + specular;
    }
    
}
