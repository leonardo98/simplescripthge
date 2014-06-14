#version 110

varying vec2 texCoord0;
varying vec4 pos;
uniform sampler2D TextureMesh;
uniform sampler2D TextureMesh2;

void main()
{
   vec4 cvet;
   vec4 cv = vec4(0.69, 0.77, 0.78, 1.0);	  
        cvet = vec4(texture2D(TextureMesh, texCoord0))+vec4(texture2D(TextureMesh2, texCoord0))*0.6;  
		if(cvet.x>0.69)
			cvet.x=0.69;
		if(cvet.y>0.77)
		   cvet.y=0.77;
		if(cvet.z>0.78)
		   cvet.z=0.78;

		   if(pos.y<-1.0)
			cvet.xyz=vec3(0.36,0.50,0.68);

			 if(pos.y>30.0)
			cvet.xyz=vec3(0.25,0.37,0.55);

   gl_FragColor.xyz = cvet.xyz;

} 