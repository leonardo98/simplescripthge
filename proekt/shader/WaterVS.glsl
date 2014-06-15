#version 110

//uniform mat4 shadow_matrix;

uniform float u_time;
varying vec4 vertex;
varying vec4 shadow_proj_coords;
varying vec4 proj_coords;

//varying vec4 v_wave;
varying vec3 v_darkColor;
varying vec3 v_lightColor;
varying float v_result;

void main()
{ 

 mat4 edmat = mat4(1.0, 0.0, 0.0, 0.0,
			       0.0, 1.0, 0.0, 0.0,
			       0.0, 0.0, 1.0, 0.0,
			       0.0, 0.0, 0.0, 1.0);
 mat4 shadow_matrix = edmat;

 vertex = gl_Vertex;
 shadow_proj_coords = shadow_matrix * vertex;

// float h = - 0.4 * abs(sin(vertex.x * 0.1 + u_time))
//  - 0.5 * abs(sin((vertex.x + vertex.z) * 0.5 + u_time * 0.8))
//  + 0.05 * sin(vertex.z * 5.5 + u_time * 3.0); 
float f = sin(vertex.x * 0.4 + u_time) * sin((vertex.x + vertex.z) * 0.5 + u_time * 0.8);
float h = - 0.1 * abs(f)
  + 0.03 * sin(vertex.z * 5.5 + u_time * 3.0)
  - 0.2 * abs(f * sin((vertex.x * vertex.x - vertex.z) * 1.6 + u_time * 1.9));
 
 vertex.y += h * 0.5;

 gl_TexCoord[0] = gl_MultiTexCoord0 * 10.0;
 gl_Position = gl_ModelViewProjectionMatrix * vertex;
 proj_coords = gl_Position;

v_darkColor = vec3(72.0/255.0, 104.0/255.0, 103.0/255.0);
v_lightColor = vec3(28.0/255.0, 68.0/255.0, 74.0/255.0);
v_result = abs(h / 0.33);

}