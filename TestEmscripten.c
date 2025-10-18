/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL OpenGL, standard IO, and, strings
#include <SDL2/SDL.h>
#undef main 

#include "GL/glew.h"
//#include <GL/GLU.h>
//#include <SDL_opengl.h>
#include <SDL2/SDL_image.h> //Needed for IMG_Load.  If you want to use bitmaps (SDL_LoadBMP), it appears to not be necessary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "map.h"
#include "misc.h"
#include "entity.h"
//#define __EMSCRIPTEN__ 1
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#define SERVER "/webgl/isometric/"
#endif
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

char ddown, ldown, rdown, udown;
char autoscroll = 0;
uint8_t n_textures = 0;
#define N_TEX_FILES 4
#define N_SHADERS 6
GLuint textures[N_TEX_FILES];
char* img_files[N_TEX_FILES]={"tilemap_3.bmp","trees.bmp","font.bmp","tilemap_3_far.bmp"};
enum shader_id {
		tiles=0,mapgen=1,mapgen2=2,entities=3,fps_display=4,upscale=5
};
GLuint shaders[N_SHADERS];
char* shader_files[N_SHADERS]={"tiles","mapgen","mapgen2","entities","fps_display","upscale"};
uint64_t last_change;
void init_entities();
void init_mapgen();
void init_fps_display();
void init_tiles();
void loadmap();

#ifndef __EMSCRIPTEN__
void ls() {
    struct dirent *de;

    DIR *dr = opendir("./res");
    if (dr == NULL) {
		return;
        printf("Could not open /res" );
    }

	char filename[32];
	struct stat filestat;
    while ((de = readdir(dr)) != NULL){
		if (de->d_name[0] == '.') continue;
		sprintf(filename,"./res/%s", de->d_name);
		stat(filename,&filestat);
		if (filestat.st_ctime > last_change){
			uint8_t i;
			for (i = 0; !strcmp(filename,img_files[i]); i++);
			printf("reload %s, %i\n", de->d_name, i);
			glDeleteTextures(1, textures+i);
			GLuint texid = loadtex(filename, i, textures+i,GL_RGB,GL_RGB);
			textures[i] = texid;
			last_change = filestat.st_ctime;
			closedir(dr);
			return;
		}
	}
    closedir(dr);    
    dr = opendir("./shaders");

    if (dr == NULL)  
    {
        printf("Could not open /shaders" );
		return;
    }

    while ((de = readdir(dr)) != NULL){
		if (de->d_name[0] == '.') continue;
		sprintf(filename,"./shaders/%s", de->d_name);
		stat(filename,&filestat);
		if (filestat.st_ctime > last_change){
				printf("change %i \n", N_SHADERS);
			uint8_t i;
			for (i = 0; i< N_SHADERS; i++){
				printf("%i\n", i);
				printf("reload %s, %i\n", shader_files[i], i);
				glDeleteShader(shaders[i]);
				char vname[32];
				char fname[32];
				sprintf(vname, "shaders/%s.vert",shader_files[i]);
				sprintf(fname, "shaders/%s.frag",shader_files[i]);
				printf("%s, %s\n", vname, fname);
				shaders[i]=createShaderProgram(
					compileShader(GL_VERTEX_SHADER,vname),
					0,
					compileShader(GL_FRAGMENT_SHADER,fname)
				);
				if (! shaders[i]) {
					printf("reloading shader %s failed", filename);
					return;
				}
				glLinkProgram(shaders[i]);
			}
			init_tiles();
			init_entities();
			init_mapgen();
			init_fps_display();
			loadmap();
			last_change = filestat.st_ctime;
		}
	}
    closedir(dr);    
}
#endif
struct uniforms { 
		GLuint tex;
		GLuint maptex;
		GLuint map_position;
		GLuint global_position;
		GLuint display_size;
} uniforms;
float   entity_grid[8];
uint32_t entities_vertex_buffer_id;
int32_t map[8];
typedef struct vec4 {
	float x;
	float y;
	float z;
	float w;
} vec4;

typedef struct entity_mesh {
	vec4 triangle1_nw;
	vec4 triangle1_no;
	vec4 triangle1_so;

	vec4 triangle2_so;
	vec4 triangle2_sw;
	vec4 triangle2_nw;
} entity_mesh;

typedef struct rect {
	float x1;
	float y1;
	float x2;
	float y2;

} rect;
int zoom = 2;
float zoom_levels[16] = {
		4,
		3,
		2,
//		1.5,
//		1.25,
		1.0,
		48.f/64,
		40.f/64,
		32.f/64,
		24.f/64,
		16.f/64,
		12.f/64,
		 8.f/64,
		 6.f/64,
		 4.f/64,
		 2.f/64,
		 1.f/64,
};
#define tile_width  32
#define tile_height 16
#define tex_width  512
#define tex_height 128
rect trees_texture_coords[] = {
		(rect) { .x1 =  36.f/tex_width, .y1 = 0, .x2 = 102.f/tex_width , .y2 = 1},
		(rect) { .x1 = 169.f/tex_width, .y1 = 0, .x2 = 226.f/tex_width , .y2 = 1},
		(rect) { .x1 =   0.f/tex_width, .y1 = 0, .x2 =  38.f/tex_width , .y2 = 1},
		(rect) { .x1 = 103.f/tex_width, .y1 = 0, .x2 = 168.f/tex_width , .y2 = 1},
		(rect) { .x1 = 227.f/tex_width, .y1 = 0, .x2 = 302.f/tex_width , .y2 = 1},
		(rect) { .x1 = 303.f/tex_width, .y1 = 0, .x2 = 371.f/tex_width , .y2 = 1},
		(rect) { .x1 = 375.f/tex_width, .y1 = 52.f/tex_height, .x2 = 406.f/tex_width , .y2 = 80.f/tex_height},
		(rect) { .x1 = (379.f)/tex_width, .y1 = 85.f/tex_height, .x2 = 411.f/tex_width , .y2 = 1},
};

entity_mesh  entity_mesh_array[MAP_WIDTH*MAP_WIDTH];
entity_mesh* entity_mesh_array_end = entity_mesh_array;

Entity entity_array[] = {
	(Entity) {
	.pos_x=10,
	.pos_y=0
	},
	(Entity) {
	.pos_x=10,
	.pos_y=0
	},
	(Entity) {
	.pos_x=10,
	.pos_y=0
	}
};
Entity* target = entity_array+0;
Entity* player = entity_array+1;
Entity* enemy  = entity_array+2;

Entity* entity_array_end= entity_array + sizeof (entity_array) / sizeof (Entity);
uint8_t map_redrawn;
//Screen dimension constants
const int PHYSICAL_SCREEN_WIDTH = 1920;
const int PHYSICAL_SCREEN_HEIGHT = 1080;
const int SCREEN_WIDTH =  1920;
const int SCREEN_HEIGHT = 1080;
#define tilemap_width 128
#define tilemap_height 64
//Starts up SDL, creates window, and initializes OpenGL
uint8_t init();

//Initializes matrices and clear color
uint8_t initGL();


//Per frame update
void update();
int framenr = 0;
void load_texture(const char * file);
void load_error(const char * file);



//Renders quad to the screen
void render();

void createVBO(GLfloat *vertexBuffer,
	GLushort *indices, GLuint numVertices, GLuint numIndices,
	GLuint* vboId, GLuint* indexId, GLuint numElems, GLenum usage);

//Frees media and shuts down SDL
void close_program();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
//UserData gUserData;

GLuint gVboId;
GLuint gIndexId;

const float DefaultNearPlaneDistance = 0.01f;
const float DefaultFarPlaneDistance = 1000.0f;
//GLuint U_map_position;
GLuint worldmap_tex_id;
GLuint worldmap_tex_id2;
//uint32_t screen_width;
//uint32_t screen_height;

#define export(func_name) __attribute__((export_name(#func_name))) func_name
float y_angle = 0.f;
SDL_Event events[100];
int n_events = 0;
int current_event=0;
//char arr[100];
//int arr_len=0;
//

#ifdef __EMSCRIPTEN__
int export(get_event_buf) (){
		return ((int)((char*)events))+ (n_events * sizeof(SDL_Event));
}
void export(event_added) (){
		n_events+=1;
}
#endif

entity_mesh* entity_row_pointers[MAP_WIDTH];

void generate_tree_objects(){
	entity_mesh_array_end = entity_mesh_array ;
	for (int y = 0; y < MAP_WIDTH  ;y++){ //make trees entities
				entity_row_pointers[y] = entity_mesh_array_end;
			for (int x= 0; x < MAP_WIDTH ; x++){
				int entity_id = worldmap_ground[
								(x*4)+
								(y*4*MAP_WIDTH)
								+1];
				if (entity_id && entity_id < 200){
					rect texture_rect = trees_texture_coords[entity_id - 1];
					float width  = 60.f/32.f * (texture_rect.x2 - texture_rect.x1) * tex_width     / SCREEN_WIDTH ;
					float height = 60.f/32.f * (texture_rect.y2 - texture_rect.y1) * tex_height    / SCREEN_HEIGHT;
					float tree_x = -width/2.f;
					float tree_y = -0.f;
					tree_x += 1.f* tile_width  * x / SCREEN_WIDTH;
					tree_x -= 1.f* tile_width  * y / SCREEN_WIDTH;
					
					tree_y += 1.f* tile_height  * x / SCREEN_HEIGHT;
					tree_y += 1.f* tile_height  * y / SCREEN_HEIGHT;
					tree_y += 1.5f* tile_height / SCREEN_HEIGHT;
								//
				*entity_mesh_array_end = (entity_mesh)	 {
					.triangle1_nw = (vec4) {.x= tree_x        , .y= tree_y - height, .z = texture_rect.x1, .w = texture_rect.y1},
					.triangle1_no = (vec4) {.x= tree_x + width, .y= tree_y - height, .z = texture_rect.x2, .w = texture_rect.y1},
					.triangle1_so = (vec4) {.x= tree_x + width, .y= tree_y         , .z = texture_rect.x2, .w = texture_rect.y2},						

					.triangle2_so = (vec4) {.x= tree_x + width, .y= tree_y         , .z = texture_rect.x2, .w = texture_rect.y2},
					.triangle2_sw = (vec4) {.x= tree_x        , .y= tree_y         , .z = texture_rect.x1, .w = texture_rect.y2},
					.triangle2_nw = (vec4) {.x= tree_x        , .y= tree_y - height, .z = texture_rect.x1, .w = texture_rect.y1},
				}; 
						entity_mesh_array_end++;
						}
					}
			}
	glUseProgram(shaders[entities]);
	glBindBuffer(GL_ARRAY_BUFFER, entities_vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof (entity_mesh) * (entity_mesh_array_end - entity_mesh_array), entity_mesh_array, GL_DYNAMIC_DRAW);
}
int poll_event(SDL_Event* e){
	if (current_event >= n_events){ 
		current_event=0;
		n_events=0;
		return 0;
	}
	*e = events[current_event];
	current_event++;
	return 400;
}
#ifdef __EMSCRIPTEN__ 
#define POLL(x) poll_event(x)
#else 
#define POLL(x) SDL_PollEvent(x)
#endif

uint8_t quit = 0;
GLuint map_framebuffer_id;
GLuint map_framebuffer_id2;
GLuint lowres_framebuffer_id;
GLuint lowres_tex_id;
void make_framebuffer(GLuint* fb_id, GLuint* tex_id,GLuint width, GLuint height){
	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, tex_id);
	glBindTexture(GL_TEXTURE_2D, *tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glGenFramebuffers(1, fb_id);
	glBindFramebuffer(GL_FRAMEBUFFER, *fb_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex_id, 0);
}
GLuint mapgen_unif_map_position;
GLuint mapgen_attr_position;
GLuint mapgen_vbuf;
GLuint mapgen_ibuf;

GLuint mapgen2_unif_map_position;
GLuint mapgen2_attr_position;
GLuint mapgen2_vbuf;
GLuint mapgen2_ibuf;
void init_mapgen(){
	mapgen_unif_map_position = glGetUniformLocation(shaders[mapgen], "map_position");
	float vxbuf[2*4] = {
		-1,-1,
		 1,-1,
		 1, 1,
		-1, 1,
	};
	GLushort indices [] = { 0, 1, 2,  2, 3, 0 };
	
	glGenBuffers(1, &mapgen_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, mapgen_vbuf);
	glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(GLfloat),vxbuf,GL_DYNAMIC_DRAW);
	glGenBuffers(1, &mapgen_ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen_ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		6 * sizeof(GLushort), indices,
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mapgen_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen_ibuf);

	glVertexAttribPointer(mapgen_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(mapgen_attr_position);
	
	mapgen2_unif_map_position = glGetUniformLocation(shaders[mapgen2], "map_position");
	

		glUniform1i(glGetUniformLocation(shaders[mapgen2], "tex"), 3);
	
	
	glGenBuffers(1, &mapgen2_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, mapgen2_vbuf);
	glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(GLfloat),vxbuf,GL_DYNAMIC_DRAW);
	glGenBuffers(1, &mapgen2_ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen2_ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		6 * sizeof(GLushort), indices,
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, mapgen2_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen2_ibuf);

	glVertexAttribPointer(mapgen2_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(mapgen2_attr_position);
}
GLuint entities_vbuf;
GLuint entities_ibuf;
GLuint entities_attr_position;
GLuint entities_tex;
void init_entities(){
glUseProgram(shaders[entities]);
	float vxbuf[2*4] = {
			0,0,
			 1,0,
			 1, 1,
			0,	1,
	};
	GLushort indices [] = { 0, 1, 2,  2, 3, 0 };
	
	glGenBuffers(1, &entities_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, entities_vbuf);
	glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(GLfloat),vxbuf,GL_DYNAMIC_DRAW);
	glGenBuffers(1, &entities_ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entities_ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		6 * sizeof(GLushort), indices,
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, entities_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entities_ibuf);


	glVertexAttribPointer(entities_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(entities_attr_position);
	entities_tex = glGetUniformLocation(shaders[entities], "tex");
	glUniform1i(entities_tex, 1);
	glUniform1i(glGetUniformLocation(shaders[entities], "maptex"), 3);
	glUniform2f(glGetUniformLocation(shaders[entities], "display_size"), SCREEN_WIDTH,SCREEN_HEIGHT);
}



GLuint fps_display_vbuf;
GLuint fps_display_ibuf;
GLuint fps_display_attr_position;
void init_fps_display(){
glUseProgram(shaders[fps_display]);
	float vxbuf[2*4] = {
			-1,    -1,
			-1.f/3,-1,
			-1.f/3, 1,
			-1, 	1,
	};
	GLushort indices [] = { 0, 1, 2,  2, 3, 0 };
	
	glGenBuffers(1, &fps_display_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, fps_display_vbuf);
	glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(GLfloat),vxbuf,GL_DYNAMIC_DRAW);
	glGenBuffers(1, &fps_display_ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fps_display_ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		6 * sizeof(GLushort), indices,
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, fps_display_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fps_display_ibuf);


	glVertexAttribPointer(fps_display_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(fps_display_attr_position);

	glUniform1i(glGetUniformLocation(shaders[fps_display], "font_tex"), 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,textures[2] );
}
void init_tiles(){
	//glUseProgram(shaders[tiles]);
	glUniform1i(uniforms.tex, 0);
	glUniform1i(uniforms.maptex, 3);
//	glUniform2f(glGetUniformLocation(shaders[tiles], "display_size"), 2.f * SCREEN_WIDTH, 2.f * SCREEN_HEIGHT);
}
void loadmap(){
	//	render();
	map_redrawn=1;
	map_position[0] = (global_position[0]/subtile) - MAP_WIDTH *0.75;
	map_position[1] = (global_position[1]/subtile) - MAP_WIDTH *0.75;
		
	glUseProgram(shaders[mapgen]);
	glUniform2f(uniforms.map_position,map_position[0], map_position[1]);
	glUniform2f(mapgen_unif_map_position,global_position[0]/subtile - MAP_WIDTH,global_position[1]/subtile - MAP_WIDTH);
	glUniform2f(uniforms.map_position,map_position[0], map_position[1]);
	glUniform2i(uniforms.global_position,global_position[0]/subtile - MAP_WIDTH,global_position[1]/subtile - MAP_WIDTH);
	
	//Before drawing
	glViewport(0,0,MAP_WIDTH*2,MAP_WIDTH*2);

//texture for framebuffer 
	glBindBuffer(GL_ARRAY_BUFFER, mapgen_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen_ibuf);


	glVertexAttribPointer(mapgen_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(mapgen_attr_position);

	glBindFramebuffer(GL_FRAMEBUFFER, map_framebuffer_id);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    

	glUseProgram(shaders[mapgen2]);
	glUniform2f(uniforms.map_position,map_position[0], map_position[1]);
	glUniform2f(mapgen2_unif_map_position,global_position[0]/subtile - MAP_WIDTH,global_position[1]/subtile - MAP_WIDTH);
	glUniform2f(uniforms.map_position,map_position[0], map_position[1]);
	glUniform2i(uniforms.global_position,global_position[0]/subtile - MAP_WIDTH,global_position[1]/subtile - MAP_WIDTH);
	
	//Before drawing

//texture for framebuffer 
	glBindBuffer(GL_ARRAY_BUFFER, mapgen2_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen2_ibuf);


	glVertexAttribPointer(mapgen2_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(mapgen2_attr_position);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,worldmap_tex_id);

	glBindFramebuffer(GL_FRAMEBUFFER, map_framebuffer_id2);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glUseProgram(shaders[entities]);
	glActiveTexture(GL_TEXTURE3);
	 
	glBindTexture(GL_TEXTURE_2D,worldmap_tex_id);

	//glUseProgram(shaders[tiles]);
	glUniform2f(uniforms.map_position,map_position[0], map_position[1]);
	//		return;
	glUniform1f(glGetUniformLocation(shaders[tiles], "zoom"),zoom);
	glActiveTexture(GL_TEXTURE3);
	 
	glBindTexture(GL_TEXTURE_2D,worldmap_tex_id2);
}
uint8_t vsync = 1;
int main(int argc, char** argv){
	if (argc > 1) vsync = !!strcmp( argv[1], "-novsync" );
	
	#ifndef __EMSCRIPTEN__
	last_change=time(0);
	#endif

	printf("press +/- to zoom\n");
	if (!init()) {
		printf("Failed to initialize!\n");
		return 0;
	}
	printf("argc: %i\n",argc);
	
	if (argc > 0){
		argv[0][0] = 0;
		printf("first argv: %s\n",argv[0]);
		printf("last  argv: %s\n",argv[argc-1]);
	}
	make_framebuffer(&map_framebuffer_id,&worldmap_tex_id,MAP_WIDTH,MAP_WIDTH);
	make_framebuffer(&map_framebuffer_id2,&worldmap_tex_id2,MAP_WIDTH,MAP_WIDTH);
	make_framebuffer(&lowres_framebuffer_id,&lowres_tex_id,960,540);
	init_entities();
	init_mapgen();
	init_fps_display();
	init_tiles();
	loadmap();
	glGenBuffers(1,&entities_vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, entities_vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, 4*4*sizeof(float)*(entity_array_end-entity_array) /*+ 128*128*sizeof(float)*/, entity_array, GL_STATIC_DRAW);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
	glEnableVertexAttribArray(0);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(render, 0, 0);
#else
	while (!quit)
	{
			render();
			SDL_GL_SwapWindow(gWindow);
	}
#endif
	glDeleteTextures(1, textures+0);

	//Free resources and close SDL
	close_program();
	return 0;
}


uint8_t init()
{
	printf("enter init()\n");
	//Initialization flag
	uint8_t success = 1;

#ifdef __EMSCRIPTEN__
	emscripten_set_canvas_element_size("#canvas", SCREEN_WIDTH, SCREEN_HEIGHT);
	EmscriptenWebGLContextAttributes attr;
	emscripten_webgl_init_context_attributes(&attr);
	attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.failIfMajorPerformanceCaveat = 0;
	attr.enableExtensionsByDefault = 1;
	attr.premultipliedAlpha = 0;
	attr.majorVersion = 2;
	attr.minorVersion = 0;
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
	emscripten_webgl_make_context_current(ctx);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("GLEW init failed: %s!\n", glewGetErrorString(err));
		success = 0;
	}

	//Initialize OpenGL
	if (!initGL())
	{
		printf("Unable to initialize OpenGL!\n");
		success = 0;
	}
#else

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = 0;
	}
	else
	{
		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = 0;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = 0;
			}
			else
			{
				//Use Vsync
				if (SDL_GL_SetSwapInterval(vsync) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				GLenum err = glewInit();
				if (GLEW_OK != err)
				{
					printf("GLEW init failed: %s!\n", glewGetErrorString(err));
					success = 0;
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = 0;
				}
			}
		}
	}
#endif
	printf("exit init()\n");
	return success;
}
#define MAXSHADERSIZE 0x10000

//GLuint programObject;
GLuint vertexShader;
void load_vshader(const char* filename, char* buf, int max){
	FILE* fp = fopen(filename, "r");
	if (fp != NULL) {
		size_t newLen = fread(buf, sizeof(char), max, fp);
		if ( ferror( fp ) != 0 ) {
			fputs("Error reading file", stderr);
		} else {
			buf[newLen++] = '\0'; /* Just to be safe. */
		}

		fclose(fp);
	}
	printf("%s\n",buf);
}

uint8_t initGL() {
	printf("enter initgl\n");
	GLuint fragmentShader;
	GLint linked;

#ifdef __EMSCRIPTEN__
	int len = 0; char *str = 0;	int err = 0;
	emscripten_wget(SERVER "shaders/tiles.frag", "shaders/tiles.frag");
	emscripten_wget(SERVER "shaders/tiles.vert", "shaders/tiles.vert");
	emscripten_wget(SERVER "shaders/mapgen.frag", "shaders/mapgen.frag");
	emscripten_wget(SERVER "shaders/mapgen.vert", "shaders/mapgen.vert");
	emscripten_wget(SERVER "shaders/mapgen2.frag", "shaders/mapgen2.frag");
	emscripten_wget(SERVER "shaders/mapgen2.vert", "shaders/mapgen2.vert");
	emscripten_wget(SERVER "shaders/entities.frag", "shaders/entities.frag");
	emscripten_wget(SERVER "shaders/entities.vert", "shaders/entities.vert");
	emscripten_wget(SERVER "shaders/fps_display.frag", "shaders/fps_display.frag");
	emscripten_wget(SERVER "shaders/fps_display.vert", "shaders/fps_display.vert");
	emscripten_wget(SERVER "shaders/upscale.frag", "shaders/upscale.frag");
	emscripten_wget(SERVER "shaders/upscale.vert", "shaders/upscale.vert");
	printf("after all shader fetch\n");	
	for (int i = 0; i < N_TEX_FILES; i++){
		char path[100] = SERVER "res/" "                           ";
		sprintf(path,"%s%s", SERVER "res/" , img_files[i]);
		printf("%s\n", path);
		emscripten_async_wget(path , img_files[i], load_texture, load_error);
	}
#endif
//	GLuint fragshader = compileShader(GL_FRAGMENT_SHADER,"shaders/tiles.frag");
	for (uint8_t i = 0; i < N_SHADERS; i++){
		char vname[32];
		char fname[32];
		sprintf(vname, "shaders/%s.vert",shader_files[i]);
		sprintf(fname, "shaders/%s.frag",shader_files[i]);
		shaders[i]=createShaderProgram(
			compileShader(GL_VERTEX_SHADER,vname),
			0,
			compileShader(GL_FRAGMENT_SHADER,fname)
		);
		if (! shaders[i]) return 0;
		glLinkProgram(shaders[i]);
	}
	printf("after all shaders compiled\n")	;

	uniforms.tex= 	glGetUniformLocation(shaders[tiles], "tex");
	uniforms.maptex = glGetUniformLocation(shaders[tiles], "maptex");
	
	uniforms.map_position = 	glGetUniformLocation(shaders[tiles], "map_position");
	uniforms.global_position = glGetUniformLocation(shaders[tiles], "global_position");
	uniforms.display_size = glGetUniformLocation(shaders[tiles], "display_size");
	printf("after some uniform definitions\n")	;

#ifndef __EMSCRIPTEN__
	for (int i = 0; i < N_TEX_FILES; i++){
		char path[100] = "res/" "                           ";
		sprintf(path,"%s%s", "res/" , img_files[i]);
		printf("%s\n", path);
		load_texture(path);
	}
#endif

	GLfloat vVertices [] = { 
		0.0f,  0.0f,   
	   -1.f/4, 1.f/8, 
		0.f,   2.f/8, 
		1.f/4, 1.f/8, 
	};
	GLushort indices [] = { 0, 1, 2, 0, 2, 3 };

	printf("before createvbo\n")	;
	createVBO(&vVertices[0],
		&indices[0], 4, sizeof(indices) / sizeof(GLushort),
		&gVboId, &gIndexId, 3, GL_DYNAMIC_DRAW);

	printf("after createvbo\n")	;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	printf("exit initgl\n");
	return 1;
}

void createVBO(GLfloat *vertexBuffer,
	GLushort *indices, GLuint numVertices, GLuint numIndices,
	GLuint* vboId, GLuint* indexId, GLuint numElems, GLenum usage)
{
	// vertex
	glGenBuffers(1, vboId);
	glBindBuffer(GL_ARRAY_BUFFER, *vboId);
	glBufferData(GL_ARRAY_BUFFER, numVertices*(sizeof(GLfloat) * numElems),
		vertexBuffer, usage);

	// bind buffer object for element indices
	glGenBuffers(1, indexId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		numIndices * sizeof(GLushort), indices,
		usage);
}

int movex=0;
int movey=0;
int lastmovex=0;
int lastmovey=0;
int speedx=0;
int speedy=0;
int last_mousemotion_frame=0;
Uint64 time1=0;
Uint64 time2=0;
uint8_t n_digits = 2;
uint32_t fps=0;
void render(){
	movex=0;
	movey=0;
	if (! (framenr %30)){
		Uint64 time2 = SDL_GetPerformanceCounter();
		fps =  1000000000 / ((time2-time1)/30);
		time1 = time2;
#ifndef __EMSCRIPTEN__
		ls();
#endif
	}
	framenr ++;
	SDL_Event e;
	while (POLL(&e)){
		switch (e.type){
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN: 
				switch (e.key.keysym.sym){
					case '+':
					case '=':
						if (zoom > 0) --zoom;
						break;
					case '-':
						if (zoom < 9) ++zoom;
						break;
				}
		    case SDL_KEYUP:
				switch (e.key.keysym.sym){
					case 'q': 
						quit=1;
						break;
					case 'j': case SDLK_DOWN:
						ddown = e.key.state;
						break;
					case 'k': case SDLK_UP:
						udown = e.key.state;
						break;
					case 'h': case SDLK_LEFT:
						ldown = e.key.state;
						break;
					case 'l': case SDLK_RIGHT:
						rdown = e.key.state;
				}
				break;
			case SDL_MOUSEWHEEL:
				zoom -= e.wheel.y;
				if(zoom < 0)zoom=0;
				else if(zoom > 9)zoom=9;
				break;
			case SDL_MOUSEBUTTONDOWN:
				speedx=0;
				speedy=0;
				break;
			case SDL_MOUSEBUTTONUP:
//				if (e.motion.state & 1){
					if (framenr - last_mousemotion_frame < 40 || last_mousemotion_frame > framenr){
						speedx=lastmovex;
						speedy=lastmovey;
						movex=0;
						movey=0;
						last_mousemotion_frame=framenr;
					}
//				}
				break;
			case SDL_MOUSEMOTION:
				if (e.motion.state & 1){
				if ((framenr - last_mousemotion_frame < 10  || last_mousemotion_frame > framenr) && (speedx!=0 || speedy!=0))break;
				movex = e.motion.xrel *4.5;
				movex += 2*e.motion.yrel *4.5;
				
				movey = -e.motion.xrel *4.5;
				movey += 2*e.motion.yrel *4.5;
				speedx=0;
				speedy=0;
				lastmovex=movex;	
				lastmovey=movey;	
				last_mousemotion_frame = framenr;
					//printf("%i %i\n", e.motion.xrel, e.motion.yrel);
				}
				if (e.motion.state & 2){
				//cmb 
				}
				if (e.motion.state & 4){
				//rmb
				}
		}
	}
	int keyboard_scrollx = 
		subtile/2.f * (-ddown + ldown  +udown - rdown) * 2/zoom_levels[zoom];
	if (keyboard_scrollx > 128) keyboard_scrollx = keyboard_scrollx & (~63);

	int keyboard_scrolly = 
		subtile/2.f * (-ddown + rdown  +udown - ldown) * 2/zoom_levels[zoom];
	if (keyboard_scrolly > 128) keyboard_scrolly = keyboard_scrolly & (~63);

	glob_pos2[0] -=-3./4 * subtile * autoscroll + 
		keyboard_scrollx +
		(movex+speedx) /zoom_levels[zoom]

	;
	glob_pos2[1] -= 2./4 * subtile * autoscroll + 
		keyboard_scrolly +
		(movey+speedy) /zoom_levels[zoom];


	if (zoom > 9){
		global_position[0] = (glob_pos2[0] & (~0x3f));
		global_position[1] = (glob_pos2[1] & (~0x3f));
	}
	if (zoom > 6){
		global_position[0] = (glob_pos2[0] & (~0xf));
		global_position[1] = (glob_pos2[1] & (~0xf));
	}
	else if (zoom > 5){
		global_position[0] = (glob_pos2[0] & (~0x7));
		global_position[1] = (glob_pos2[1] & (~0x7));
	}
	else if (zoom >= 2){
		global_position[0] = (glob_pos2[0] & (~0x3));
		global_position[1] = (glob_pos2[1] & (~0x3));
	}else {
		global_position[0] = (glob_pos2[0]);
		global_position[1] = (glob_pos2[1]);
	}
	global_position[0] = (glob_pos2[0] & (~0x07));
	global_position[1] = (glob_pos2[1] & (~0x07));

	speedx *= .9;
	speedy *= .9;
	if (//1||
		global_position[0]/subtile - map_position[0] < MAP_WIDTH * 5/10  || 
		global_position[0]/subtile - map_position[0] >= MAP_WIDTH || 

		global_position[1]/subtile - map_position[1] < MAP_WIDTH * 5/10  || 
		global_position[1]/subtile - map_position[1] >= MAP_WIDTH   
	){
		printf("loadmap\n");
		loadmap();
	}

// draw lowres
//	glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	glDisable(GL_BLEND);	
	//glUseProgram(shaders[tiles]);
	if (zoom_levels[zoom] >= 2){
		glBindFramebuffer(GL_FRAMEBUFFER, lowres_framebuffer_id);
		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0,0,SCREEN_WIDTH / zoom_levels[zoom],SCREEN_HEIGHT / zoom_levels[zoom]);
		glUniform2f(uniforms.display_size,SCREEN_WIDTH / zoom_levels[zoom], SCREEN_HEIGHT / zoom_levels[zoom]);
	}else {
		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUniform2f(uniforms.display_size,SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	glBindBuffer(GL_ARRAY_BUFFER, mapgen_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen_ibuf);

	glUniform2f(uniforms.map_position,map_position[0], map_position[1]);
	glUniform2i(uniforms.global_position,global_position[0], global_position[1]);
	uint32_t positionLoc = glGetAttribLocation(shaders[tiles],"position");
	glVertexAttribPointer(positionLoc, 2, GL_FLOAT,
					   GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(positionLoc);
	glUniform1f(glGetUniformLocation(shaders[tiles], "zoom"),zoom_levels[zoom]);
	glActiveTexture(GL_TEXTURE0);
	if (zoom_levels[zoom] >= 1.f){
		glBindTexture(GL_TEXTURE_2D, textures[0]);
	}else {
		glBindTexture(GL_TEXTURE_2D, textures[3]);
	}
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,worldmap_tex_id2 );
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	//if (zoom_levels[zoom] < 2){
	//	//glUseProgram(shaders[tiles]);
	//	glDisable(GL_BLEND);	
	//}
	//else {
	if (zoom_levels[zoom] >= 2){
		glUseProgram(shaders[upscale]);	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, mapgen_vbuf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapgen_ibuf);
		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D,lowres_tex_id );
		glUniform1f(glGetUniformLocation(shaders[upscale], "zoom"),zoom_levels[zoom]);
		glUniform1i(glGetUniformLocation(shaders[upscale], "tex"),3);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	
	}


	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,worldmap_tex_id2 );

	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shaders[entities]);
	
    glUniform2i(glGetUniformLocation(shaders[entities], "global_position"),global_position[0],global_position[1]);
	glUniform2f(glGetUniformLocation(shaders[entities], "map_position"),map_position[0],map_position[1]);
	glUniform1f(glGetUniformLocation(shaders[entities], "zoom"),zoom_levels[zoom]);

	glBindBuffer(GL_ARRAY_BUFFER, entities_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entities_ibuf);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,textures[1] );
	glUniform1i(entities_tex, 1);

	glVertexAttribPointer(entities_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(entities_attr_position);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, 512*512);



#ifndef __EMSCRIPTEN__
	
	glViewport(SCREEN_WIDTH-150,SCREEN_HEIGHT-100,120,80);
	glUseProgram(shaders[fps_display]);

	glBindBuffer(GL_ARRAY_BUFFER, fps_display_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fps_display_ibuf);

	glUniform1i(glGetUniformLocation(shaders[fps_display], "fps"), fps);
	n_digits = 2+(fps>=100);

	glVertexAttribPointer(fps_display_attr_position, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), (const void*) 0);
	glEnableVertexAttribArray(fps_display_attr_position);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textures[2] );
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, n_digits);

#endif


	//glUseProgram(shaders[tiles]);
	glDisable(GL_BLEND);	

	///////////////
}


void close_program()
{
	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void load_texture(const char * file)
{
    GLuint texid = loadtex(file, n_textures, textures+n_textures,GL_RGB,GL_RGB);

	textures[n_textures] = texid;
	++n_textures;
}

void load_error(const char * file)
{
	printf("File download failed: %s", file);
}


