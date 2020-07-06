//these vars come from the vertex shader
varying vec4 v_color;
varying vec3 v_eye;
varying vec4 v_Ia;
varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_uv;
varying vec3 v_world_position;

//varying vec3 v_light; //position of the light source
//varying vec3 v_Id; //diffuse intensity of light source
//varying vec3 v_Is; //specular intensity of light source

//the textures passed from the application
uniform sampler2D u_texture;

void main()
{
	//read the pixel RGBA from the texture at the position v_uv
	vec4 color = texture2D( u_texture, v_uv );

	//here we set the parameters
	vec3 P = v_world_position;
	vec3 N = normalize( v_normal );
	//vec3 L = normalize( v_light - P );
	//vec3 V = normalize( v_eye - P );
	//vec3 R = normalize( reflect( -L, N ) );
	vec3 K = color.xyz;
	//float specular = color.w;
	//float alpha = 30.0;

	//here write the computations for PHONG.
	vec3 La = K * v_Ia.xyz;
	//vec3 Ld = K * max(0.0,dot(L,N)) * v_Id;
	//vec3 Ls = specular * K * pow( max(0.0,dot(R,V)), alpha ) * v_Is;
	
	color = vec4(La, 1.0);
	//assign the color to the output
	//gl_FragColor = vec4(La + Ld + Ls, 1.0);
	//gl_FragColor = vec4(La, 1.0);
	float dist = length( v_world_position - v_eye ) * 0.004;
	gl_FragColor =  v_Ia*mix( color, vec4(0.61960784313,0.74901960784,0.79607843137,1.0), vec4(clamp(dist,0.0,1.0)));
}
