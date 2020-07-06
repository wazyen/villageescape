
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform vec3 u_camera;

void main()
{
	vec2 uv = v_uv;
	vec4 color = u_color * texture2D( u_texture, uv );

	//color.xyz = vec3(1.0) - color.xyz;
	color.xyz = vec3(color.x) * vec3(0.0,1.0,0.0);
	//color += 0.5 * texture2D( u_texture, uv );

	gl_FragColor =  color;
}
