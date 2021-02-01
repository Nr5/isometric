/*
 *  Created on: Feb 24, 2018
 */

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
SDL_Surface img;

uint32_t tree_texture;
uint32_t texture;
int tree_shader;
int shader;
int mapgen_shader;
SDL_Window *sdlwin=0;
int32_t* map;
uint8_t worldmap[256*256];
uint8_t worldmap2[256*256];

typedef struct game_object {
	float x;
	float y;
	uint8_t id;

} game_object;
typedef struct rect {
	float x1;
	float y1;
	float x2;
	float y2;

} rect;
#define tex_width 440
#define tex_height 102
rect trees_texture_coords[] = {
		(rect) { .x1 =   0.f/tex_width, .y1 = 0, .x2 =  40.f/tex_width , .y2 = 1},
		(rect) { .x1 =  40.f/tex_width, .y1 = 0, .x2 = 103.f/tex_width , .y2 = 1},
		(rect) { .x1 = 103.f/tex_width, .y1 = 0, .x2 = 183.f/tex_width , .y2 = 1},
		(rect) { .x1 = 183.f/tex_width, .y1 = 0, .x2 = 247.f/tex_width , .y2 = 1},
		(rect) { .x1 = 247.f/tex_width, .y1 = 0, .x2 = 325.f/tex_width , .y2 = 1},
		(rect) { .x1 = 325.f/tex_width, .y1 = 0, .x2 = 400.f/tex_width , .y2 = 1},
		(rect) { .x1 = 400.f/tex_width, .y1 = 30.f/tex_height, .x2 = 440.f/tex_width , .y2 = 55.f/tex_height},
		(rect) { .x1 = 400.f/tex_width, .y1 = 55.f/tex_height, .x2 = 440.f/tex_width , .y2 = 1},
};

game_object game_object_array[256*256];
game_object* end_game_object = game_object_array;

int subtile = 64;


SDL_Renderer* sdl_renderer;
SDL_GLContext glcontext;
int window_id;
float mouse_position[2]={0,-1};
int32_t  global_position[2]={128*64,128*64};
int32_t  map_position[2]={0,0};


int32_t target[2];
char ldown,rdown,udown,ddown;
int screen_width=800;
int screen_height=600;

uint32_t worldmap_tex_id;

unsigned int buffer;
unsigned int buffer2;

float vertices[] ={
	-1,1,	
	-1, -1,	
	1, -1,	
	1,1,	
};

int v2[]={
	  0,0,
	-20,10,
	0,20,
	 20,10
};


typedef struct Player{
	int32_t pos_x;
	int32_t pos_y;
}Player;
Player player={
.pos_x=10,
.pos_y=0
};
uint8_t size[2];
void draw(){
	if (SDL_GL_MakeCurrent(sdlwin,  glcontext)){
		printf( "'isometric.c' Error: change glcontext failed\n");
	}
	
	glUseProgram(shader);
	glEnable(GL_TEXTURE_2D);
	
	int texLoc = glGetUniformLocation(shader, "tex");
	glUniform1i(texLoc, 0);

	texLoc = glGetUniformLocation(shader, "maptex");
	glUniform1i(texLoc, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,worldmap_tex_id );

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	
	glColor3f(1,1,1);
	
	//Draw ground
	///////////////
	glDrawArraysInstanced(GL_TRIANGLE_FAN,0, 4, 128*128);
	///////////////
	glUseProgram(0);

	
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
		float target_screenpos[2] = { target[0] -global_position[0], target[1] - global_position[1]};
		float player_screenpos[2] = { player.pos_x -global_position[0], player.pos_y - global_position[1]};
		
		glColor3f(1,1,0);		
		float x =- (20.f/(80 * subtile)) * ( target_screenpos[1] - target_screenpos[0] )/ ((float)screen_width / 200.f ) ;
		float y =- (10.f/(60 * subtile)) * ( target_screenpos[1] + target_screenpos[0] )/ ((float)screen_height / 150.f ) ;
		
		glVertex3f(x - 20.f/80  / ((float)screen_width / 200.f ) ,y-2-    10.f/60.f / ((float)screen_height / 150.f ) 	,0);
		glVertex3f(x             								 ,y-2 							,0);
		glVertex3f(x + 20.f/80  / ((float)screen_width / 200.f ) ,y-2-    10.f/60.f / ((float)screen_height / 150.f ) 	,0);
		glVertex3f(x            								 ,y-2- 2*(10.f/60.f / ((float)screen_height / 150.f )) 	,0);

		
		glColor3f(1,0,0);		

		x =- (20.f/(80 * subtile)) * ( player_screenpos[1] - player_screenpos[0] )/ ((float)screen_width / 200.f ) ;
		y =- (10.f/(60 * subtile)) * ( player_screenpos[1] + player_screenpos[0] )/ ((float)screen_height / 150.f ) ;
		
		glVertex3f(x - 20.f/80.f / ((float)screen_width / 200.f )   ,y-2-     10.f/60.f / ((float)screen_height / 150.f ) 	,0);
		glVertex3f(x             									,y-2 				                         			,0);
		glVertex3f(x + 20.f/80.f / ((float)screen_width / 200.f )   ,y-2-     10.f/60.f / ((float)screen_height / 150.f ) 	,0);
		glVertex3f(x            									,y-2- 2*( 10.f/60.f / ((float)screen_height / 150.f )) 	,0);

	glEnd();
	
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,tree_texture );
	//glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		float xshift = (global_position[0]/subtile -map_position[0]) ;
		float yshift = (global_position[1]/subtile -map_position[1]) ;
		
		float xshift2 =- (20.f/80) * ( yshift - xshift )/ ((float)screen_width / 200.f ) ;
		//i don't know what 1.87 below  means
		float yshift2 =2.03- (10.f/60) * ( yshift + xshift )/ ((float)screen_height / 150.f ) ;
		for (game_object* go = game_object_array; go < end_game_object; go++){
			float width = trees_texture_coords[go->id].x2 - trees_texture_coords[go->id].x1;
			float height = trees_texture_coords[go->id].y2 - trees_texture_coords[go->id].y1;
			glColor4f(1,1,1,1);
			
			glTexCoord2f(trees_texture_coords[go->id].x1,trees_texture_coords[go->id].y2);
			glVertex2f(go->x - width /4 - xshift2, go->y - yshift2);
			
			glTexCoord2f(trees_texture_coords[go->id].x2,trees_texture_coords[go->id].y2);
			glVertex2f(go->x + width /4 - xshift2, go->y - yshift2);
			
			glTexCoord2f(trees_texture_coords[go->id].x2,trees_texture_coords[go->id].y1);
			glVertex2f(go->x + width /4 - xshift2, go->y + height/4- yshift2);
			
			glTexCoord2f(trees_texture_coords[go->id].x1,trees_texture_coords[go->id].y1);
			glVertex2f(go->x - width /4 - xshift2, go->y + height/4 - yshift2);
		}
	glEnd();
	glDisable(GL_BLEND);	


	glUseProgram(shader);
	SDL_GL_SwapWindow(sdlwin);

}

void loadmap(){
	map_position[0] = global_position[0]/subtile - 128;
	map_position[1] = global_position[1]/subtile - 128;
	glUseProgram(mapgen_shader);
	glUniform2uiv(glGetUniformLocation(mapgen_shader,"map_position"),1,map_position);
	
	//Before drawing

	glViewport(0,0,256,256);
	
	glBegin(GL_QUADS);
	glColor4f(1,1,1,1);
	glVertex3f(-1,-1,0);
	glVertex3f( 1,-1,0);
	glVertex3f( 1, 1,0);
	glVertex3f(-1, 1,0);
	glEnd();
	
	glReadPixels(0,0,256,256,GL_RED,	GL_UNSIGNED_BYTE,worldmap);
	glReadPixels(0,0,256,256,GL_GREEN,	GL_UNSIGNED_BYTE,worldmap2);
	end_game_object = game_object_array;
	for (int y = 0; y < 256; y++){
			for (int x = 0; x < 256; x++){
				if (worldmap2[x + y* 256] ){
					float x2 =- (20.f/80 ) * ( y - x) / ((float)screen_width / 200.f ) ;
					float y2 =- (10.f/60 ) * ( y + x) / ((float)screen_height / 150.f ) ;
					
					*end_game_object = (game_object)	 {.x = x2 , .y = y2, .id = worldmap2[x + y * 256] - 1 };
					end_game_object++;
				}
			}
	}
	printf("number of game_objects: %i\n" , end_game_object - game_object_array);
	glUseProgram(shader);

	glViewport(0,0,screen_width, screen_height);
	glUniform2uiv(glGetUniformLocation(shader,"map_position"),1,map_position);
	glActiveTexture(GL_TEXTURE1);
 	 
        
    glGenTextures(1,&worldmap_tex_id);
    glBindTexture(GL_TEXTURE_2D,worldmap_tex_id);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,256,256,0,GL_RED,GL_UNSIGNED_BYTE,worldmap);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}
int main(int argc, char** argv){
	int sdlflags=0;	

	SDL_Init(SDL_INIT_EVERYTHING);
	if (argc>2){
		screen_width=atoi(argv[1]);
		screen_height=atoi(argv[2]);
	}else{
		//sdlflags |= SDL_WINDOW_FULLSCREEN;

		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		screen_width = DM.w;
		screen_height = DM.h;
		
	
		printf("screenwidth: %i, screenheight: %i\n",screen_width,screen_height);
	
	}
	if (argc>3 && argv[3][0] == 'f' ){
		sdlflags |= SDL_WINDOW_FULLSCREEN;
	}

	
	sdlwin=SDL_CreateWindow("win" , 0,  0, screen_width,screen_height,  
													SDL_WINDOW_OPENGL|
													SDL_WINDOW_ALWAYS_ON_TOP | 
													SDL_WINDOW_RESIZABLE|
													SDL_WINDOW_INPUT_FOCUS|
													SDL_WINDOW_BORDERLESS|
													sdlflags);
	SDL_RaiseWindow(sdlwin);	
	if (!sdlwin) {
        printf("Could not create window: %s\n", SDL_GetError());
    }
    
    sdl_renderer =SDL_CreateRenderer(sdlwin,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

	glcontext = SDL_GL_CreateContext(sdlwin);
	window_id= SDL_GetWindowID( sdlwin ); 
	
	//SDL_SetVideoMode(1920,960,16,SDL_OPENGL);
	//glOrtho(0,800,600,0,0,1);

	if(glewInit() != GLEW_OK){
		printf("glewinit failed\n");
		//exit(1);
	}
	player.pos_x=80 * subtile;
	player.pos_y=80 * subtile;
	
	target[0]=80 * subtile;
	target[1]=80 * subtile;

	

	player.pos_x=0x1221040;
	player.pos_y=0x1221040;
	
	target[0]=0x1221040;
	target[1]=0x1221040;
	
	global_position[0] += 0x700100A;	
	
  	SDL_Surface* img=SDL_LoadBMP("../res/texmap2.bmp");
  	SDL_Surface* tree_img=SDL_LoadBMP("../res/trees.bmp");
	
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1,&tree_texture);
	glBindTexture(GL_TEXTURE_2D ,tree_texture);
	
	glTexImage2D(GL_TEXTURE_2D ,0,GL_RGBA,tree_img->w,tree_img->h,
						0,GL_BGRA,GL_UNSIGNED_BYTE,tree_img->pixels);
	

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			
	glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE,GL_ZERO);

	tree_shader=createShaderProgram(
		0,
		0,//compileShader(GL_GEOMETRY_SHADER,readfile("basic.gs")),
		compileShader(GL_FRAGMENT_SHADER,"./basic.frag")
	);
	
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D ,texture);
	
	glTexImage2D(GL_TEXTURE_2D ,0,GL_RGB,img->w,img->h,
						0,GL_BGR,GL_UNSIGNED_BYTE,img->pixels);
	

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			
	glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE,GL_ZERO);

	shader=createShaderProgram(
		compileShader(GL_VERTEX_SHADER,"./basic.vert"),
		0,//compileShader(GL_GEOMETRY_SHADER,readfile("basic.gs")),
		compileShader(GL_FRAGMENT_SHADER,"./basic.frag")
	);

	mapgen_shader=createShaderProgram(
		0,
		0,
		compileShader(GL_FRAGMENT_SHADER,"./mapgen.frag")
	);



	loadmap();

	//GLuint renderBuffer;
	//GLuint frameBuffer;

	printf("open file \n");
	FILE* fp =fopen("./mapmap.map","r");
	printf("open file \n");
	
	
	fread(size,1,2,fp);
	
	
	
	printf("width: %i\nheight: %i\n",size[0],size[1]);
	
	//map=malloc(size[0]*size[1]*sizeof(int)  + 8*sizeof(int) );
	map=malloc(128*128*sizeof(int)  + 8*sizeof(int) );

	((float*)map)[0]=0;
	((float*)map)[1]=0;
	
	((float*)map)[2]=-20.f/80.f;
	((float*)map)[3]= 10.f/60.f;
	
	((float*)map)[4]= 0;
	((float*)map)[5]= 20.f/60.f;
	
	((float*)map)[6]= 20.f/80.f;
	((float*)map)[7]= 10.f/60.f;
	
	
	
	
	printf("map: \n");
	//for(int i=8;i<size[0]*size[1]+8;i++){
	//	map[i]=0;
	fread( (uint8_t*)(map+8), size[0]*size[1], 1, fp);
	//	((float*)map)[i]=(float)map[i];
	//}
	
	
	glGenBuffers(1,&buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(int) + 128*128*sizeof(float), map, GL_STATIC_DRAW);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
	
	glEnableVertexAttribArray(0);
	

	
	glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,0,0);
	
	glEnableVertexAttribArray(1);
	
	glVertexAttribDivisor(1, 1);
	
	
	
	
	glUniform1i(glGetUniformLocation(shader,"width"),128	);
	glUniform2f(glGetUniformLocation(shader,"display_size"),screen_width,screen_height	);
	glUniform2uiv(glGetUniformLocation(shader,"map_position"),1,map_position);
	


	for (int j=0 ; j<size[1] ; j++){
		for (int i=0 ; i < size[0] ; i++){
	//		worldmap[j*256 + i] =  ((uint8_t*)map+8)[ j*size[0] + i];

		}

	}
	
	
//	for (int i=0; i<256*128;i++ ){
//		printf("%i ", worldmap[i]);
//	}
	printf("\n");
	glViewport(0,0,screen_width,screen_height);
	printf("1\n");
	glUseProgram(shader);

	char running=1;
	//////////////
	// Game Loop
			global_position[0] = (player.pos_x ) +50 * subtile;
			global_position[1] = (player.pos_y ) +50 * subtile;
	while(running)
	{
		
		if (
				global_position[0]/subtile - map_position[0] < 100  || 
				global_position[0]/subtile - map_position[0] > 216  || 
				global_position[1]/subtile - map_position[1] < 100  || 
				global_position[1]/subtile - map_position[1] > 216  

						){
			loadmap();
			printf("global_pos: %i,%i\n",global_position[0] , global_position[1]);
			printf("map_pos: %i,%i\n",map_position[0]  ,map_position[1]);
			printf("target: %i,%i\n",target[0]  ,target[1]);
		}

		SDL_Event event;
		
		while (SDL_PollEvent(&event)){
		if(event.type==SDL_MOUSEBUTTONUP){
			target[0] =((int)(subtile*mouse_position[0])+global_position[0]) & ~(subtile - 1) ;
			target[1] =((int)(subtile*mouse_position[1])+global_position[1]) & ~(subtile - 1) ;
				
		}
		if(event.type==SDL_MOUSEMOTION){
			
			float x = 1.f *  event.motion.x -screen_width/2;// / 400.f -1;
			float y = 1.f * (event.motion.y -screen_height*3/2)*2;// / 300.f -1;
			
			x*= 4.f/5.f;	
			y*= 4.f/5.f;
		
			
	

			mouse_position[0]=(x * 0.353562 + y*0.353545 )   /14.142135623730951  ;
			mouse_position[1]=(x * -0.707090 + y*0.707123)   /14.142135623730951 /2;

			//printf("mouse_pos: (%i, %i)\n",(int)(mouse_position[0])+(int)(global_position[0]),(int)(mouse_position[1])+(int)(global_position[1]));	
		
			glUniform2fv(glGetUniformLocation(shader,"mouse_position"),1,mouse_position);	


		}
		else if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.sym == SDLK_LEFT){
				ldown=1;
			}
			if (event.key.keysym.sym == SDLK_RIGHT){
				rdown=1;
			}

			if (event.key.keysym.sym == SDLK_UP){
				udown=1;
			}
			if (event.key.keysym.sym == SDLK_DOWN){
				ddown=1;
			}
		}
		else if (event.type == SDL_KEYUP){
			if (event.key.keysym.sym == SDLK_LEFT){
				ldown=0;
			}
			if (event.key.keysym.sym == SDLK_RIGHT){
				rdown=0;
			}

			if (event.key.keysym.sym == SDLK_UP){
				udown=0;
			}
			if (event.key.keysym.sym == SDLK_DOWN){
				ddown=0;
			}

			if (event.key.keysym.sym == SDLK_ESCAPE){
				running=0;
			}


		}
		else if(event.type==SDL_QUIT){
			running=0;

		}
		else if(event.type==SDL_WINDOWEVENT){
		//	printf("windowevent\n");
			if (event.window.event == SDL_WINDOWEVENT_RESIZED){
				
				screen_width = event.window.data1;
				screen_height = event.window.data2;
				
			//	printf("w,h= (%i,%i) \n",screen_width,screen_height);
				glViewport(0, 0, screen_width, screen_height);
				glUniform2f(glGetUniformLocation(shader,"display_size"),screen_width,screen_height	);
			}
		}
		

		}
		global_position[0] -= 2*subtile * (-ddown + ldown  +udown - rdown);
		global_position[1] -= 2*subtile * (-ddown + rdown  +udown - ldown);
		
		glUniform2i(glGetUniformLocation(shader,"global_position"),global_position[0],global_position[1]);
		
		{
		//////////////
		// move
		uint32_t dx = target[0] - player.pos_x;
		uint32_t dy = target[1] - player.pos_y;
		int distance = sqrt( dx * dx + dy * dy );
	
		if (distance >2){
			int new_x = player.pos_x+( (target[0] - player.pos_x) /(distance/16))  ;
			int new_y = player.pos_y+( (target[1] - player.pos_y) /(distance/16))  ;
			if ( 
				(int)(worldmap[(int) ((new_y / subtile) - map_position[1]) * 256 +((int) player.pos_x / subtile) - map_position[0]] ) 	&&
				(int)(worldmap[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * 256 +((int) player.pos_x / subtile) - map_position[0]] ) 	&&
				(int)(worldmap[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * 256 +(int) ((player.pos_x / subtile) - map_position[0] + !!(player.pos_x % subtile)) ] ) &&
				(int)(worldmap[(int) ((new_y / subtile)     - map_position[1]) * 256 +  (int) ((player.pos_x / subtile) - map_position[0] + !!(player.pos_x % subtile)) ] )	&&
				
				!(int)(worldmap2[(int) ((new_y / subtile) - map_position[1]) * 256 +((int) player.pos_x / subtile) - map_position[0]]  ) 	&&
				!(int)(worldmap2[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * 256 +((int) player.pos_x / subtile) - map_position[0]] ) 	&&
				!(int)(worldmap2[(int) ((new_y / subtile + !!(new_y % subtile) ) - map_position[1]) * 256 +(int) ((player.pos_x / subtile) - map_position[0] + !!(player.pos_x % subtile)) ] ) &&
				!(int)(worldmap2[(int) ((new_y / subtile)     - map_position[1]) * 256 +  (int) ((player.pos_x / subtile) - map_position[0] + !!(player.pos_x % subtile)) ] )
			   ){
				player.pos_y=new_y;
			}
			if ( 
				(int)(worldmap[(int) ((player.pos_y / subtile) - map_position[1])     * 256 +(int) (new_x / subtile) - map_position[0]] ) 	&&
				(int)(worldmap[(int) ((player.pos_y / subtile) - map_position[1] +  !!(player.pos_y % subtile) ) * 256 +(int) (new_x / subtile) - map_position[0]] ) 	&&
				(int)(worldmap[(int) ((player.pos_y / subtile) - map_position[1] +  !!(player.pos_y % subtile) ) * 256 +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]  ) &&
				(int)(worldmap[(int) ((player.pos_y / subtile) - map_position[1] )    * 256 +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]  )
			&&	
				!(int)(worldmap2[(int) ((player.pos_y / subtile) - map_position[1])     * 256 +(int) (new_x / subtile) - map_position[0]]  ) 	&&
				!(int)(worldmap2[(int) ((player.pos_y / subtile) - map_position[1] +  !!(player.pos_y % subtile) ) * 256 +(int) (new_x / subtile) - map_position[0]]  ) 	&&
				!(int)(worldmap2[(int) ((player.pos_y / subtile) - map_position[1] +  !!(player.pos_y % subtile) ) * 256 +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]) &&
				!(int)(worldmap2[(int) ((player.pos_y / subtile) - map_position[1] )    * 256 +(int) (new_x / subtile - map_position[0] +  !!(new_x % subtile) ) ]  )
				){	
				player.pos_x=new_x;
			}
		
		}else{
			player.pos_x=target[0];
			player.pos_y=target[1];
		}
//			global_position[0] = (player.pos_x ) +50 * subtile;
//			global_position[1] = (player.pos_y ) +50 * subtile;
		}



		draw();

		//glRotated(1,0,1,0);
	}
	SDL_Quit();
}
