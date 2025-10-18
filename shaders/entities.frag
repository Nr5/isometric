uniform sampler2D tex;
in vec2 tex_coord;
out vec4 col;
void main(){
	col=texture(tex,tex_coord).bgra;
	if (col == vec4(1.f)){
		col.a = 0.f;	
	}
}
