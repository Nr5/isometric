uniform sampler2D font_tex;
in vec2 tex_coord;
out vec4 col;
void main(){
	//col=vec4(1);//texture(font_tex,tex_coord).bgra;
	col = texture(font_tex,tex_coord/vec2(2.f,-1.f)).brrr;
}
