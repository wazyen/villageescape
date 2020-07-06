
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
	float dist = length( v_world_position - u_camera ) * 0.004;
	vec4 color = u_color * texture2D( u_texture, uv );
	gl_FragColor =  u_color*mix( color, vec4(0.61960784313,0.74901960784,0.79607843137,1.0), vec4(clamp(dist,0.0,1.0)));
}
