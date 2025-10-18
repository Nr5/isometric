in vec2 position;

uniform sampler2D tex;
uniform sampler2D maptex;
uniform vec2 map_position;
uniform ivec2 global_position;
uniform vec2 display_size;
uniform float zoom;
out vec2 tex_coord;
int mapwidth = 512;

const float tex_width = 512.f;
const float tex_height = 128.f;
//const float display_size.x = 1920.f;
//const float display_size.y = 1080.f;
int subtile = 64;
float tilewidth = 32.f;
float tileheight = 16.f;
float tiletexwidth = 256.f;
float tiletexheight = 128.f;

float entity_tex_coords[9*4] = float[36](
		0.f,0.f,0.f,0.f,
		  36.f/tex_width, 1.f, 102.f/tex_width , 0.f,
		 169.f/tex_width, 1.f, 226.f/tex_width , 0.f,
		   0.f/tex_width, 1.f,  38.f/tex_width , 0.f,
		 103.f/tex_width, 1.f, 168.f/tex_width , 0.f,
		 227.f/tex_width, 1.f, 302.f/tex_width , 0.f,
		 303.f/tex_width, 1.f, 371.f/tex_width , 0.f,
		 375.f/tex_width, 80.f/tex_height, 406.f/tex_width , 52.f/tex_height,
		 (379.f)/tex_width, 1.f, 411.f/tex_width , 85.f/tex_height
);
float positions[9*4] = float[36](
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f
 );


void main(){
	positions = float[36](0.f,0.f,0.f,0.f,
		  (36.f-102.f)   /display_size.x, 0.f, (102.f-36.f) /display_size.x ,-tex_height/display_size.y,
		  (169.f-226.f)  /display_size.x, 0.f, (226.f-169.f)/display_size.x ,-tex_height/display_size.y,
		  (0.f-38.f)     /display_size.x, 0.f, (38.f-0.f)   /display_size.x ,-tex_height/display_size.y,
		  (103.f-168.f)  /display_size.x, 0.f, (168.f-103.f)/display_size.x ,-tex_height/display_size.y,
		  (227.f-302.f)  /display_size.x, 0.f, (302.f-227.f)/display_size.x ,-tex_height/display_size.y,
		  (303.f-371.f)  /display_size.x, 0.f, (371.f-303.f)/display_size.x ,-tex_height/display_size.y,
		  (375.f-406.f)  /display_size.x, 0.f, (406.f-375.f)/display_size.x ,-28.f/display_size.y     ,
		  (379.f-411.f)  /display_size.x, 0.f     , (411.f-379.f)/display_size.x/2.f ,-43.f/display_size.y

);
float mapposx = float(float(map_position.x) - float(global_position.x)/ float(subtile) );
float mapposy = float(float(map_position.y) - float(global_position.y)/ float(subtile) );
		int x = (gl_InstanceID%mapwidth) - int(mapposx)-512;
		int y = (gl_InstanceID/mapwidth) - int(mapposy)-512;
vec4 map_val = texture(maptex,
  		 vec2(x,y)/1024.f
  		 );
	//if (map_val.g == 0.f){
	//		return;

	//}
		//map_val= vec4(1.,1./256,0.,1.);
  	//tex_coord = entity_tex_coords[int(map_val.g*256.)].zw * position;
  	tex_coord = vec2( 
				entity_tex_coords[
						int(map_val.g*256.f)*4 +
						int(position.x)*2
				],
				entity_tex_coords[
						int(map_val.g*256.f)*4 +
						int(position.y)*2 + 1
				]
		);
  	vec2 pos = vec2( 
				positions[
						int(map_val.g*256.f)*4 +
						int(position.x)*2
				],
				positions[
						int(map_val.g*256.f)*4 +
						int(position.y)*2 + 1
				]
		);
gl_Position = vec4(0,0,0,1);

vec2 tile_pos = vec2(
				(float(x)*tilewidth -  float(y)*tilewidth)/display_size.x,
				(float(x)*tileheight + float(y)*tileheight)/display_size.y
				);
gl_Position.xy = (pos*vec2(1,2) + tile_pos +
vec2(
	tilewidth  * (mapposx-mapposy) / display_size.x,
	tileheight * (mapposx+mapposy) / display_size.y
)
		);//*zoom;
//	gl_Position=vec4(vec2(-1,-1)+ pos +
//					 vec2(float(gl_InstanceID%mapwidth)*.1,float(gl_InstanceID/mapwidth )*.1),
//					 0.f,1.f);
gl_Position *= vec4 (1.f,-1.f,0.f,1.f);
gl_Position += vec4 (0.f,-8.f,0.f,0.f);
gl_Position.y += -2.f* tileheight/display_size.y;
gl_Position.xy*=zoom;
}
