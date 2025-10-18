//#version 330 core
uniform sampler2D tex;
uniform vec2 map_position;
out vec4 out_color;
float hash(float n) { return fract(sin(n) * 10000.f); }
float hash(vec2 p) { return fract(10000.f * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

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

//
// TODO the tree generation uses perlin noise, maybe white noise would be more appropriate
int n_trees=8;
void main(){	
	vec2 mappos_float = vec2(map_position);
	vec2 glob_pos = mappos_float + gl_FragCoord.xy;
//glob_pos = glob_pos + vec2(noise(glob_pos),noise(glob_pos.yx))*4;
	

		//gl_Position=vec4(position,-.999,0,1);
	float tc2= -20.f + noise( glob_pos  *.007f )*30.f;
	tc2 = max(tc2,-4.f);
	
	tc2 += noise(glob_pos  *.001f + vec2(.4) )*3.f;
	
		tc2 += noise(glob_pos  *.01f )*7.f-2.f;
//	tc2 += noise(glob_pos  *.03f )*10.f;
		
	tc2     += noise(glob_pos  * 1.f )*.5;
 	float tc3 = 0.f;
	tc2     += noise(glob_pos  * .5f )*.01f;
	tc2 = clamp (tc2,0.f,2.f)	;
	tc2 += noise(glob_pos  * .1f )*.5f;
		if (tc2 > 0.8){

		tc2 += max(0.f,noise(glob_pos  *.004f )*1.4f-.8);
		}
	float tc4 = noise(glob_pos  *.08 ) * 2.f	;
	tc4 += noise(glob_pos  *.05 ) * 2.f	;
	//	tc2 = clamp (tc2,-1.f,3.2f)	;
	if ( tc2 > 2.0f ) { 
//	tc3 = fract(sin(
//					dot(
//							glob_pos, 
//							vec2(  
//									float(int(glob_pos.y) % 16) / 17.f, 
//									float(int(glob_pos.x) % 15) / 16.f 
//							)
//					)
//			)*53.8856) *100.f / tc2 / tc2 / tc2 / tc2 ;
		tc3 = noise(glob_pos  *1.f ) * tc2 * tc2 /2.5f - 1.8	;
		if (tc3 < 2.f/256.f )tc3 = 0.f;
	
		if (tc3 >= float(n_trees)/float(256-1) ) tc3 = 0.f;
	}
	
	else if (tc2 <= 1.7 && tc2 >= 1.3f){

		tc3 = fract(sin(
						dot(
								glob_pos, 
								vec2(  
										float(int(glob_pos.y) % 16) / 17.f, 
										float(int(glob_pos.x) % 15) / 16.f 
								)
						)
				)*53.8856) *6.f / tc2 / tc2;
		if (tc3 >= 2.f/float(256-1) ) tc3 = 0.f;
	}
			//tc3 = (( (int (glob_pos.x)%512) * (int(glob_pos.y)%256)) % 16)/20.f/tc2 ;//tc3 =  noise( vec2( gl_FragCoord.xy + mappos_float)) * .5 / tc2 ;
	
	if (tc3 >= float(n_trees)/float(256-1) ) tc3 = 0.f;
	if (tc4 < .67 && tc2 > 2.f) {
			float prevtc2 = tc2;
			if (tc4 < .6)	{
					tc2 = tc4; 
					tc3 = 0.f;
			}
			if (tc4 > .58 ) {
					if (int (tc2 * tc4 * tc2)%2 != 0) {
							tc3 = 8.f/float(256-1); // if at side of lake change to seagrass;
					}
					else if (prevtc2 < 3.f && int (tc4 * tc4 * tc2 * 8.f)% 2 != 0 )tc2 = 1.f;
					else if (tc2 == 2.5) tc2 = 2.f;
					
			}
	}
	

//	if (gl_FragCoord.x + map_position.x < 297025) tc3 = 1.f/255;
//	if (gl_FragCoord.y + map_position.y < 297025) tc3 = 1.f/255;

	tc2 	= tc2 * 2.f;
//	tc2      = min(9.f,tc2);
	//	tc2 = floor(tc2);
tc2 *=3.;
tc2 -= 5.f;
tc2 = max(tc2,0.);
float	flower_noise = noise( (gl_FragCoord.xy + mappos_float)  * .3f  + vec2(tc2*5.f,tc2*2.f));


	if (flower_noise > .8 && fract(tc2)<.1 &&  int(tc2+.2) == int(tc2) ) tc2 = tc2+ 32.f;
		else {
		float	flower_noise2 = noise( (gl_FragCoord.xy + mappos_float)  * .4f  + vec2(tc2*4.f,tc2*2.f));


			if (flower_noise2 > .9 && fract(tc2)<.1 &&  int(tc2+.2) == int(tc2) ) tc2 = tc2+ 48.f;
		}



	//if (tc2 > 6.f/256)tc2 = 6.f/256;
if (tc3!=0.f)tc2=15.f;
if (tc2 < 16.f)tc2 = min(tc2, 10.);
tc2      = tc2 / 256.f;
out_color = vec4(max(0.f,tc2),
				tc3,
				 0.f,1.f );
}
