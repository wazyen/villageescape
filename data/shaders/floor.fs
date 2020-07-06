varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture_grass;
uniform sampler2D u_texture_rocks;
uniform sampler2D u_texture_mask;
uniform float u_time;
uniform vec3 u_camera;

void main()
{
	vec2 uv = v_uv;
	float mask = texture2D(u_texture_mask, uv).x;
	vec4 grass = texture2D(u_texture_grass, uv * 100.0);
	vec4 rocks = texture2D(u_texture_rocks, uv * 1000.0);
	vec4 color = mask * grass + ( 1.0 - mask ) * rocks;
	float dist = length( v_world_position - u_camera ) * 0.004;
	
	gl_FragColor =  u_color * mix( color, vec4(0.61960784313,0.74901960784,0.79607843137,1.0), vec4(clamp(dist,0.0,1.0)));
}
