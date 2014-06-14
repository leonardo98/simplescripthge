#version 110

//uniform mat4 shadow_matrix;

uniform float u_time;
varying vec4 vertex;
varying vec4 shadow_proj_coords;
varying vec4 proj_coords;

void main()
{ 

 mat4 edmat = mat4(1.0, 0.0, 0.0, 0.0,
			       0.0, 1.0, 0.0, 0.0,
			       0.0, 0.0, 1.0, 0.0,
			       0.0, 0.0, 0.0, 1.0);
 mat4 shadow_matrix = edmat;

 vertex = gl_Vertex;
 shadow_proj_coords = shadow_matrix * vertex;

 float h = 0.2 * sin(vertex.x * 0.1 + u_time)
  + 0.25 * sin((vertex.x + vertex.z) * 0.5 + u_time * 0.8)
  + 0.05 * sin(vertex.z * 5.5 + u_time * 3.0); 

 vertex.y += h * 0.5;

 gl_TexCoord[0] = gl_MultiTexCoord0;
 gl_Position = gl_ModelViewProjectionMatrix * vertex;
 proj_coords = gl_Position;
}