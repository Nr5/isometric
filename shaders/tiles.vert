in vec2 position;
int tile_tex_width = 256;
int tile_tex_height = 128;
float tile_height = 16.f;
float tile_width = 32.f;

int width = 1024;

uniform sampler2D tex;
uniform vec2 map_position;
uniform ivec2 global_position;// =ivec2(10000,10000);
uniform float zoom;
uniform vec2 display_size;
//vec2 display_size = vec2(1920.f,1080.f);


uniform sampler2D maptex; 


out vec2 tex_coord;
int subtile = 64;

void main(){
	gl_Position = vec4(position,0,1);

//gl_Position.xy += vec2( 
//	float(global_position.x % subtile)/(display_size.x)-
//	float(global_position.y % subtile)/(display_size.x)
//
//	, 
//	float(global_position.x % subtile)/(display_size.y)+
//	float(global_position.y % subtile)/(display_size.y)
//)*zoom/4;
	//tex_coord=position/zoom;
	tex_coord.y  =(display_size.x/2.f) / (tile_width/2.f) *  max(0.f,position.x);
	tex_coord.x  =(display_size.x/2.f) / (tile_width/2.f) * -min(0.f,position.x);
	
	tex_coord.x +=(display_size.y)   / (tile_height) * (position.y+1.f)/2.f;
	tex_coord.y +=(display_size.y)   / (tile_height) * (position.y+1.f)/2.f;
	
//	tex_coord /= 256.f;
	tex_coord=-tex_coord;
	tex_coord+= vec2(64);
//	tex_coord /= zoom;
	tex_coord+= vec2(754);

	tex_coord.x -= float(map_position.x) - float(global_position.x) / float(subtile) ;
	tex_coord.y -= float(map_position.y) - float(global_position.y)/ float(subtile) ; 
	if (zoom == 2.f ) {
		tex_coord-= vec2(32,32);
	}
	if (zoom == 3.f) {
		tex_coord-= vec2(43,43);
	}
	if (zoom == 4.f) {
		tex_coord-= vec2(48,48);
	}
//	tex_coord *=16.f;
//tex_coord.x = float(int(tex_coord.x ) )	;
//tex_coord.y = float(int(tex_coord.y ) )	;
	return;
		float tiles_per_tilemap_height = float(tile_tex_height) / tile_height;
		float tiles_per_tilemap_width =  float(tile_tex_width) / tile_width;
//		gl_Position=vec4(a_position,1);
		//gl_Position= vec4(position,0,1);
float x = -(float(gl_InstanceID % width ) + float(map_position.x) - float(global_position.x/ subtile ));
float y = -(float(gl_InstanceID / width ) + float(map_position.y) - float(global_position.y/ subtile )); 
//x = float(int(x ) )	;
//y = float(int(y ) )	;

//int biome[20]=int[20](9,0,1,11,3,10, 9, 9, 9, 9, 9,0,1,11,3,10,9,9,9,9);
//int biome[20]=int[20]( 11,0,1, 7,3,12,11,11,11,11,11,0,1,7 ,3,12,11,11,11,11);
int biome[16]=int[16]( 0,1,1,1,1,2,2,3,4,4,4,5,5,5,5,5);
float tilewidth = 32.f;
float tileheight = 16.f;

float mapposx = x+ float(float(map_position.x) - float(global_position.x)/ float(subtile) );
float mapposy = y+ float(float(map_position.y) - float(global_position.y)/ float(subtile) );

		gl_Position=
  vec4(
	vec2(0.,0.0)+
	(
		position * tilewidth *4.f / vec2(display_size.x, display_size.y)
		+ vec2(
			tilewidth  * (mapposx-mapposy) / display_size.x,
			tileheight * (mapposx+mapposy) / display_size.y
		)
	)	
			
	   ,0.f,1.f);


gl_Position *= vec4 (1.0,-1.0,0,1);
gl_Position += vec4 (0.0,-8.f,0,0);
gl_Position.xy*=zoom;

int tc2 =(int( texture(maptex,  vec2(x+.5,y+.5) / float(width)  ).r*  256.f  )    );

vec2 pos = position;
pos = pos / vec2(4.f,2.f); 

int tc_nextx =(int( texture(maptex,  vec2(x+1.+.5,y+.5) / float(width)  ).r*  256.f  )    ) %16;
int tc_nexty =(int( texture(maptex,  vec2(x+.5,y+1.+.5) / float(width)  ).r*  256.f  )    ) %16;

if (tc_nextx == tc2%16 && tc_nexty == tc2%16 && tc2 >= 16){
		tc_nexty = -tc2/16;
		tc2 = tc2 % 16;
		tc2=biome[tc2];
}
else {
		tc2 = tc2 % 16;
		tc2=biome[tc2];
		tc_nextx=biome[tc_nextx];
		tc_nexty=biome[tc_nexty];

		//if (tc2==3)tc2 = 0;
		//if (tc_nextx==3)tc_nextx = 0;
		//if (tc_nexty==3)tc_nexty = 0;

		//tc2 = gl_InstanceID % 5; 
		if (tc_nextx != tc2){
				if (tc_nextx == tc_nexty){
					pos.x= abs(pos.x);				
				}
				pos.x = -pos.x;
				tc_nexty = tc_nextx;
		}
}
if (tc_nexty == tc2 || tc_nexty < 0){
		float seedx = -(float(gl_InstanceID % width )- float(global_position.x/ subtile) );
		float seedy = -(float(gl_InstanceID / width )- float(global_position.y/ subtile) ); 
		if (fract (float (seedx + seedy) / 3.1415) > .5){
			pos.x = -pos.x;
		}
}
pos = pos - vec2(0.f,1.f/16.f);
pos = pos * vec2(14.f/16.f,14.f/16.f);
pos = pos + vec2(0.f,1.f/16.f);
tex_coord= float(tc2)*
				vec2(1.f/tiles_per_tilemap_width,0) + 
				float(tc_nexty) * 
				vec2(-1.f/tiles_per_tilemap_width/2.f, 1.f/tiles_per_tilemap_height/2.f) +
				pos + vec2(1.f/tiles_per_tilemap_height /2.0,0.f) ;
//if (tc2==8)tex_coord=vec2(0.f);
//if (tc2 >= 8) tex_coord+= vec2(-.5,0);

}
