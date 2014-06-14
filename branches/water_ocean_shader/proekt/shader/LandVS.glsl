#version 110

varying vec2 texCoord0;
varying vec4 pos;

void main()
{  
   texCoord0 = gl_MultiTexCoord0.xy;  
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
   pos = gl_Vertex;   
}