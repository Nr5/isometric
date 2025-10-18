//#version 330 core
uniform sampler2D tex;
uniform vec2 map_position;
out vec4 out_color;
in vec2 pos;
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
int map_width=1024;
void main(){	
	//vec4 bla = texture(tex,gl_FragCoord.xy/vec2(1920.f,1080.f));

	out_color = texture(tex,pos);
int near_trees = 0;
if (out_color.r > 7.f/256.f){
	for (float y = -2.; y <=2. && near_trees < 1; y++){
		for (float x = -2.; x <=2. && near_trees < 1; x++){
			if (abs(x) + abs(y) == 4.f) continue;
			int tree = int(texture(tex,(gl_FragCoord.xy + vec2(x,y)) / vec2(map_width)  ).g*257.f);
			if (tree > 3 && tree <  7){
				near_trees ++;
			}
		}
	}
	if (near_trees > 0) {
		out_color.r = (float(int(out_color.r*256.f)/16)*16.f +  15.f)/256.f;
	}

	for (float y = -4.; y <=4. && near_trees < 2; y++){
		for (float x = -4.; x <=4. && near_trees < 2; x++){
			if (abs(x) + abs(y) > 6.f) continue;
			int tree = int(texture(tex,(gl_FragCoord.xy + vec2(x,y)) / vec2(map_width)  ).g*257.f);
			if (tree > 2 && tree <  7){
					near_trees ++;
			}
		}
	}
	if (near_trees > 1) {
		out_color.r = (float(int(out_color.r*256.f)/16)*16.f +  15.f)/256.f;
	}

	for (float y = -6.; y <=6. && near_trees < 4; y++){
		for (float x = -6.; x <=6. && near_trees < 4; x++){
			if (abs(x) + abs(y) > 10.f) continue;
			int tree = int(texture(tex,(gl_FragCoord.xy + vec2(x,y)) / vec2(map_width)  ).g*257.f);
			if (tree > 2 && tree <  7){
					near_trees ++;
			}
		}
	}
	if (near_trees > 3) {
		out_color.r = (float(int(out_color.r*256.f)/16)*16.f +  15.f)/256.f;
	}

}

}
