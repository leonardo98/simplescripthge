#version 110

varying vec2 texCoord0;
varying vec3 normal;
varying vec4 pos;

uniform sampler2D texturebox;
uniform vec4 post;
uniform vec4 cr;
uniform vec4 poss;
uniform vec4 lp;

void main()
{ 
   vec4 lpos2 = vec4(0.0, 4.0, -3.0, 1.0);
   vec4 lpos1 = vec4(0.0, 4.0, 3.0, 1.0);

   vec3 n = normalize(normal);
   vec4 fuse = vec4(0.52,0.52,0.52,1.0);
    
    
   vec4 ec = normalize(lp - pos);
   float ht = max(dot(n, ec.xyz), 0.0);
   vec4 e = fuse * cr * ht;
   
   vec4 ec1 = normalize(lpos2 - pos);
   float ht1 = max(dot(n, ec1.xyz), 0.0);
   vec4 e1 = fuse * cr * ht1;
   
   
   
   vec4 mbient = vec4(0.32,0.32,0.32,1.0);
   vec4 olor = vec4(0.32,0.32,0.32,1.0);
   vec4 t = mbient * olor;
   
   float ss = 90.0;
   vec4 ve = vec4(0.12,0.12,0.12,1.0);
   vec4 vv = normalize(poss - pos);
   vec4 halfVec = normalize(ec + vv);
   
   float ght = pow(max(dot(n, halfVec.xyz), 0.0), ss);
   vec4 pec = vec4(0.82,0.82,0.82,1.0);
   if(ht <= 0.0) ght = 0.0;
   vec4 ar = pec * cr * ght;
   gl_FragColor  = (ve + t + (e + e1) + ar) * texture2D(texturebox, texCoord0);
   gl_FragColor.w = 1.0;
}