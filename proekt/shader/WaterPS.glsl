uniform sampler2D normal_texture;

//varying vec2 v_bumpUv1;
varying vec3 v_darkColor;
varying vec3 v_lightColor;
varying float v_result;

//varying vec2 v_worldPos;

void main()
{
    vec4 normalMapValue = texture2D(normal_texture, gl_TexCoord[0]);
    gl_FragColor.rgb = mix(v_lightColor, v_darkColor, (normalMapValue.x * v_result) + (normalMapValue.y * (1.0 - v_result)));
	gl_FragColor.a = 1.0;
//    vec4 normalMapValue = texture2D(normal_texture, gl_TexCoord[0]);
//    gl_FragColor = vec4(mix(v_lightColor, v_darkColor, (normalMapValue.x * v_wave.y) + (normalMapValue.y * (1.0 - v_wave.y))), v_wave.x)
//		+ exp2(log2(((normalMapValue.z * v_wave.y) + (normalMapValue.w * (1.0 - v_wave.y))) * v_wave.z) * v_wave.w + v_reflectionPower);
}