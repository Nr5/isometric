#version 330 core
uniform sampler2D tex;
uniform uvec2 global_position;
uniform uvec2 map_position;


float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(float x) {
	float i = floor(x);
	float f = fract(x);
	float u = f * f * (3.0 - 2.0 * f);
	return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}



void main(){
	//gl_Position=vec4(position,-.999,0,1);
	float tc2= -1 + noise(vec2(gl_FragCoord.xy + map_position.xy /*% uvec2(0x10000)*/ )  *.01f )*3;
	tc2     += noise(vec2(gl_FragCoord.xy + map_position.xy /*% uvec2(0x10000)*/ )  * .1f )*1;
	tc2      = min(2.5,tc2);
	tc2      = tc2 * 2.f / 256;
	//if (tc2 > 6.f/256)tc2 = 6.f/256;
 	gl_FragColor = vec4(tc2,noise(vec2(gl_FragCoord.xy + map_position.xy)),0,0 );
	
/*
	if (gl_FragCoord.x < 4 ){
 		gl_FragColor = vec4( (int(gl_FragCoord.x)%6)/256.f) ;

	}
	if (gl_FragCoord.y < 4 ){
 		gl_FragColor = vec4( (int(gl_FragCoord.x)%6)/256.f) ;
	}

/*
	if (gl_FragCoord.x < 2 ){
 		gl_FragColor = vec4(0);
		
	}

	if (gl_FragCoord.y < 2 ){
 		gl_FragColor = vec4(0);
		
	}

*/
	
	
	
	
}
