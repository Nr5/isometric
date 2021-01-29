#version 330 core
uniform sampler2D tex;
varying vec2 col;

void main(){
	
	gl_FragColor=texture(tex,col);

}
