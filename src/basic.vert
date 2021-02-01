#version 330 core
layout (location=0)in vec2 position;
layout (location=1)in float tc;
uniform sampler2D tex;
uniform sampler2D maptex; 

uniform int width;
uniform uvec2 map_position;
uniform ivec2 global_position;
uniform vec2 mouse_position;
uniform vec2 display_size;

varying vec2 col;
int subtile = 64;

void main(){

	float x = -(gl_InstanceID % (width ) + 1.f*map_position.x -1.f*(global_position.x/ subtile) );
	float y = -(gl_InstanceID / (width ) + 1.f*map_position.y -1.f*(global_position.y/ subtile) ); 
	
	int biome[10];
	biome[0] = 9;
	biome[1] = 0;
	biome[2] = 1;
	biome[3] = 11;
	biome[4] = 3;
	biome[5] = 10;
	biome[6] = 3;
	biome[7] = 2;
	biome[8] = 6;
	biome[9] = 2;
							


	//int width=10;
	gl_Position=
	  vec4(
		vec2(0,2.0)+
		(position + 
		-vec2( 20.f/(80 * subtile), 10.f/(60 * subtile))* (subtile*int(gl_InstanceID % width) + (global_position.x) % subtile ) +
		-vec2(-20.f/(80 * subtile), 10.f/(60 * subtile))* (subtile*int(gl_InstanceID / width) + (global_position.y) % subtile ) 
		)*vec2(1.f / (display_size.x/200.f)   , (float(display_size.x)/display_size.y)* 3/4 /(display_size.x/200.f)  )
		
	,0,1);
	gl_Position*=vec4(1,-1,0,1);

	
	int tc2=(int( texture(maptex,  vec2(x+.5,y+.5) / 256.f  ).r*  256.0  )    );
	
	tc2=biome[tc2];

	col= tc2*vec2(.25,1.f/6) + position + vec2(.25,0);

}
