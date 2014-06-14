#version 110

uniform sampler2D normal_texture; // карта нормалей
uniform sampler2D refract_texture; // карта преломлений (сцены под водой)
uniform sampler2D reflect_texture; // карта отражений (сцены над водой)
uniform sampler2D depth_texture; // карта глубины сцены
uniform sampler2DShadow shadow_texture; // карта глубины сцены с позиции источника света
uniform sampler2D texture_r; //края

uniform vec4 time_density_clipplane;
uniform vec3 light_source;//
uniform vec3 view_position;//
uniform vec4 water_color;//
uniform vec3 specular_color;//

varying vec4 vertex;
varying vec4 shadow_proj_coords;
varying vec4 proj_coords;

const float fNormalScale = 1.0; 

float SampleShadow(vec3 shadow_tc)
{
  return shadow2D(shadow_texture, shadow_tc).x;
}

float calculate_linear_depth(float value)
{
  return time_density_clipplane.w * time_density_clipplane.z / 
    ( time_density_clipplane.w - value * 
    (time_density_clipplane.w - time_density_clipplane.z) );
}

void main()
{

 // gl_FragColor = vec4(1.0);
 // return;
 vec3 normal = 2.0 * texture2D(normal_texture, gl_TexCoord[0].st * fNormalScale 
  + vec2(time_density_clipplane.x) ).xzy - vec3(1.0);
  // так как в карте нормалей значения заданы в касательном пространстве,
  // то нам нужно перевести его в мировое
  // поэтому .xzy, а не .xyz
  normal = normalize(normal + vec3(0.0, 1.0, 0.0));
  // немного «выпрямим» нормаль (это делать не обязательно)

  // введем переменную для обозначения
  // масштабного коэффициента HDR изображения
  float fHDRscale = water_color.w;
  // вычислим нормированное положение источника света
  vec3 lpnorm   = normalize(light_source);
  // вычислим нормированный вектор взгляда
  vec3 vpnorm   = normalize(view_position - vertex.xyz);
  // вычислим проективные координаты
  vec3 proj_tc  = 0.5 * proj_coords.xyz / proj_coords.w + 0.5;
  // вычислим коэффициент Френеля для смешивания отражения и преломления
  float fresnel = 1.0 - dot(vpnorm, normal);


  // вычисляем расстояние от камеры до точки 
 float fOwnDepth = calculate_linear_depth(proj_tc.z);
 // считываем глубину сцены
 float fSampledDepth = texture2D(depth_texture, proj_tc.xy).x;
 // преобразуем её в линейную (расстояние от камеры)
 fSampledDepth       = calculate_linear_depth(fSampledDepth);
 // получаем линейную глубину воды
 float fLinearDepth  = fSampledDepth - fOwnDepth;


 float fExpDepth = 1.0 - exp( -time_density_clipplane.y * fLinearDepth);
 float fExpDepthHIGH = 1.0 - exp( -0.95 * fLinearDepth );

 vec3 shadow_tc = shadow_proj_coords.xyz / shadow_proj_coords.w + 
    0.06 * vec3(normal.x, normal.z, 0.0);
  // вычисляем текстурные координаты со смещением
  float fShadow = SampleShadow(shadow_tc); // получаем непосредственную тень
  float fSurfaceShadow = 0.25 * fShadow + 0.75; // осветляем её


   vec3 ref_vec = reflect(-lpnorm, normal); // вычисляем отраженный вектор
 // получаем скалярное произведение 
 // отраженного вектора на вектор взгляда
 float VdotR = max( dot(ref_vec, vpnorm), 0.0 );  
 // аппроксимация Шлика a^b = a / (b – a*b + a) для a от нуля до единицы
 VdotR /= 1024.0 - VdotR * 1024.0 + VdotR;
 vec3 specular = specular_color * vec3(VdotR) * fExpDepthHIGH * fShadow; 
 // вычисляем отраженный свет, с учетом затенения и глубины воды в данной точке.

 /////////////////////
 // величина искажения – чем глубже, тем искажения больше
  float fDistortScale = 0.1 * fExpDepth;
  vec2 vDistort = normal.zx * fDistortScale; // смещение текстурных координат
  // читаем глубину в искаженных координатах
  float fDistortedDepth = texture2D(depth_texture, proj_tc.xy + vDistort).x;
  // преобразуем её в линейную
  fDistortedDepth = calculate_linear_depth(fDistortedDepth);
  float fDistortedExpDepth = 
    1.0 - exp( -time_density_clipplane.y * (fDistortedDepth - fOwnDepth) );
  // вычисляем экспоненциальную глубину в искаженных координатах
  // теперь сравниваем расстояния – если расстояние до воды больше,
  // чем до прочитанной то пренебрегаем искажением
  if (fOwnDepth > fDistortedDepth) 
  {
    vDistort = vec2(0.0);
    fDistortedExpDepth = fExpDepth;
  }
  // теперь читаем из текстуры преломлений цвет
  vec3 refraction = texture2D(refract_texture, proj_tc.xy + vDistort).xyz;
  // и закрашиваем его цветом воды, в зависимости от глубины
  refraction = mix(refraction, water_color.xyz * fHDRscale, fDistortedExpDepth);


  // коэффициент можно подобрать под свои нужды
 vDistort = normal.xz * 0.025;
 vec3 reflection = texture2D(reflect_texture, proj_tc.st + vDistort).xyz;


  float fMix = fresnel * fExpDepthHIGH; 
  // вычисляем коэффициент смешивания
  // как коэффициент Френеля плюс вычисленная глубина
  // для плавного перехода из «берега» в воду
  // смешиваем и умножаем на осветленную тень
  vec3 result_color = mix(refraction, reflection, fMix) * fSurfaceShadow;

  result_color += specular; 
  //if(vertex.x>10.0)
	result_color.xyz = (result_color.xyz + vec4(texture2D(texture_r, gl_TexCoord[0].xy)).xyz)*1.2;
			if(result_color.x>0.69)
			result_color.x=0.69;
		if(result_color.y>0.77)
		   result_color.y=0.77;
		if(result_color.z>0.78)
		   result_color.z=0.78;

 /////////////////////

  
  gl_FragColor = vec4( result_color, 1.0); // ура! записываем значение


}