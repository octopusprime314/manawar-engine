#version 430

uniform vec3 lightPos;
uniform float farPlane;
out float fragColor;
in vec4 FragPos;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
} 