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


int n_trees=8;
void main(){
	//gl_Position=vec4(position,-.999,0,1);
	float tc2= -1 + noise(vec2(gl_FragCoord.xy + map_position.xy /*% uvec2(0x10000)*/ )  *.01f )*5;
 	float tc3 = 0;
	tc2     += noise(vec2(gl_FragCoord.xy + map_position.xy /*% uvec2(0x10000)*/ )  * .1f )*2;
	
	float tc4 = noise(vec2(gl_FragCoord.xy + map_position.xy /*% uvec2(0x10000)*/ )  *.08 ) * 2	;
	tc4 += noise(vec2(gl_FragCoord.xy + map_position.xy /*% uvec2(0x10000)*/ )  *.05 ) * 2	;
	if ( tc2 > 2 ) tc3 =  noise( vec2( gl_FragCoord.xy + map_position.xy)) * .5 / tc2 ;
	if (tc3 >= n_trees/255.f ) tc3 = 0;
	if (tc4 < .67 && tc2 > 2) {
			float prevtc2 = tc2;
			if (tc4 < .6)	{
					tc2 = tc4; 
					tc3 = 0;
			}
			if (tc4 > .58 ) {
					if (int (tc2 * tc4 * tc2)%2 != 0) {
							tc3 = 8.f/255; // if at side of lake change to seagrass;
					}
					else if (prevtc2 < 3 && int (tc4 * tc4 * tc2 * 8)% 2 != 0 )tc2 = 1;
					else if (tc2 == 2.5) tc2 = 2;
					
			}
	}
	

//	if (gl_FragCoord.x + map_position.x < 297025) tc3 = 1.f/255;
//	if (gl_FragCoord.y + map_position.y < 297025) tc3 = 1.f/255;

	tc2      = min(4.5,tc2);
	tc2      = tc2 * 2.f / 256;
	//if (tc2 > 6.f/256)tc2 = 6.f/256;

	gl_FragColor = vec4(tc2,
					tc3
					,0,0 );
	
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
