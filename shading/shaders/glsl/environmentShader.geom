#version 430 
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 viewMatrices[6];
out vec4 FragPos; // FragPos from GS (output per emitvertex)

in VsData
{
	vec3 normal;
    vec2 textureCoord;
}  vsData[3];

out GeomData
{
	vec3 normal;
    vec2 textureCoord;
}  geomData;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = viewMatrices[face] * FragPos;
			geomData.textureCoord = vsData[i].textureCoord;
			geomData.normal = vsData[i].normal;
            EmitVertex();
        }    
        EndPrimitive();
    }
}