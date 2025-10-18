/*
 * misc.h
 *
 *  Created on: Jan 27, 2018
 */

#ifndef MISC_H_
#define MISC_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#ifdef __EMSCRIPTEN__
char shaderbuf[0x10000]= "#version 300 es\nprecision mediump float;\n";
char shader_headersize = 41;
#else
char shaderbuf[0x10000]= "#version 330 core\n";
char shader_headersize = 18;
#endif
static unsigned int compileShader(unsigned int type, const char* filename){

    FILE* fp = fopen(filename,"r");
	
	int sz = fread(shaderbuf+shader_headersize, 1, 0x10000, fp);
	shaderbuf[sz+shader_headersize]=0;
 	const char* src2=shaderbuf;
	printf("%s\n", filename);
	printf(type==GL_VERTEX_SHADER?"vertex ":type==GL_FRAGMENT_SHADER?"fragment ":"geometry ");
	
	
	unsigned int id= glCreateShader(type);
	if (!id){
		printf( "Shader failed\nShader type not supported\n");
		printf( "OpenGL Version: %s\n", glGetString(GL_VERSION) );
		return 0;
	}
	
	
	
	glShaderSource(id,1,&src2,0);
	glCompileShader(id);

	int result;
	glGetShaderiv(id,GL_COMPILE_STATUS, &result);

	if (!result){
		printf("Shader failed\n");

		int length;
		glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);

		char* message= (char*)alloca(10000);

		glGetShaderInfoLog(id,length,&length,message);
		printf("%s\n\n",message);

		glDeleteShader(id);
		return 0;
	}
	printf( "Shader compiled.\n\n");
	
//	free(src);
	fclose(fp);
	
	return id;
}

static unsigned int createShaderProgram(unsigned int vertexshader,unsigned int geometryshader,unsigned int fragmentshader){
	unsigned int program = glCreateProgram();
if (!vertexshader || !fragmentshader) return 0;
glAttachShader(program,vertexshader);
if(geometryshader)	glAttachShader(program,geometryshader);
glAttachShader(program,fragmentshader);

	glLinkProgram(program);
	glValidateProgram(program);
/*
	glDeleteShader(vertexshader);
	glDeleteShader(geometryshader);
	glDeleteShader(fragmentshader);
*/
	printf("program created\n\n\n");
	return program;

}
int loadtex(const char* filename, uint32_t gl_tex_nr, uint32_t* tex_id, uint32_t color_mode1, uint32_t color_mode2){
  	SDL_Surface* tree_img = SDL_LoadBMP(filename);
	printf("%s: ", filename);
	if (tree_img == NULL) {
		printf("Error: \"%s\"\n", SDL_GetError()); return 0;
	}	
	printf("loaded (%dx%d)\n", tree_img->w, tree_img->h);
//	glActiveTexture(gl_tex_nr);
	glGenTextures(1,tex_id);
	glBindTexture(GL_TEXTURE_2D ,*tex_id);
	
	glTexImage2D(GL_TEXTURE_2D ,0, color_mode1, tree_img->w,tree_img->h,
						0,color_mode2,GL_UNSIGNED_BYTE,tree_img->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			
	glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	SDL_FreeSurface(tree_img);
	return *tex_id;
}
/*
static unsigned int loadTexture(const char* filename){
	SDL_Surface* img=SDL_LoadBMP(filename);
	unsigned int id;
	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_2D,id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,img->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	SDL_FreeSurface(img);
	std::cout <<"texture loaded\n";
	return id;
}
*/
#endif /* MISC_H_ */
