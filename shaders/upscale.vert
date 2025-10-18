in vec2 position;
out vec2 tex_coord;
uniform float zoom;
void main(){
	tex_coord = (position/2.f + vec2(.5))/zoom*2.f;
	gl_Position = vec4(position,0.f,1.f);
}
