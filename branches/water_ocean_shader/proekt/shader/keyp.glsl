#version 110

uniform sampler2D myTexture;
varying vec2 vTexCoord;
void main () 
{  
   vec4 color = texture2D(myTexture, vTexCoord); 
             
//   if ((color[0]  > 0.99)&(color[2]  > 0.99))
//      discard; 
   if (color.rgb == vec3(0.0,0.0,0.0))
      discard; 
   
   gl_FragColor = color*color*color;
} 