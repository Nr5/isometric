uniform sampler2D tex;
uniform sampler2D maptex; 
uniform vec2 display_size;
uniform ivec2 global_position;// =ivec2(10000,10000);
uniform float zoom;
int tile_tex_width = 256;
int tile_tex_height = 128;
float tile_height = 16.f;
float tile_width = 32.f;

in vec2 tex_coord;
out vec4 col;
void main(){
	float tiles_per_tilemap_height = float(tile_tex_height) / tile_height;
	float tiles_per_tilemap_width =  float(tile_tex_width) / tile_width;
	float tile_id=texture(maptex,tex_coord/1024.f).r*256.f;
	float biome[16]=float[16]( 0.,1.,1.,1.,1.,2.,2.,3.,4.,4.,4.,5.,5.,5.,5.,5.);
	float variation = floor(tile_id/16.f);
	tile_id = biome[(int(tile_id)%16)];
	
	float next_tile_y=biome[int(texture(maptex,(tex_coord+vec2(0,1))/1024.f).r*256.f)];
	float next_tile_x=biome[int(texture(maptex,(tex_coord+vec2(1,0))/1024.f).r*256.f)];
	float next_tile_xy=biome[int(texture(maptex,(tex_coord+vec2(1,1))/1024.f).r*256.f)];
	//col = vec4(floor(fract(tex_coord).x * 32)/32,floor(fract(tex_coord).y * 32)/32,0,1);

	vec2 pos_rel_to_tile = fract(tex_coord) / vec2(tiles_per_tilemap_width,tiles_per_tilemap_height);
	pos_rel_to_tile =vec2((pos_rel_to_tile.r-pos_rel_to_tile.g)/1.f,pos_rel_to_tile.r+ pos_rel_to_tile.g )/2.f;
	float flip = 1.f;
	float variation2;
	if (pos_rel_to_tile.x > 0.f && next_tile_y == tile_id){
		variation2 = -next_tile_x;
	} else {
		variation2 = -next_tile_y;
	}

	if (next_tile_x != tile_id && (next_tile_y == tile_id || pos_rel_to_tile.x > 0.f) ){
		variation2 = -next_tile_x;
		flip = -1.f;
	}
//	if (next_tile_x == tile_id && next_tile_y == tile_id && next_tile_xy != tile_id){
		if (fract(tex_coord).x > 0.5 &&  fract(tex_coord.y) > 0.5){
			variation2 = -next_tile_xy;
		}
//	}
	if (variation2 != tile_id){
		variation = variation2;
	}
	vec2 tile_tex_coord = 
	tile_id * vec2(1.f/tiles_per_tilemap_width,0.f) + 
	1.f * vec2(1.f/tiles_per_tilemap_width,0.f) + 
	vec2(flip,1.f) * pos_rel_to_tile - vec2(.50f/tiles_per_tilemap_width,0.f)+
	vec2(.5f/tiles_per_tilemap_width,-.5f/tiles_per_tilemap_height) * (variation)
	;
	col = texture(tex,tile_tex_coord).bgra;
}
