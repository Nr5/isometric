in vec2 position;
out vec2 tex_coord;
uniform int fps;
void main(){
	gl_Position = vec4(position+vec2(1.3f-2.f * float(gl_InstanceID)  /3.f,0.f),0.f,1.f);
		tex_coord=position;//*3.f/2.f/2.f+vec2(.5);
		tex_coord.x += 1.f;
		tex_coord.x *= 3.f;
		tex_coord.x *= 8.f/128.f;
		if (gl_InstanceID == 0){
			tex_coord.x += float(fps % 10) * 16.f/128.f;
		} 
		if (gl_InstanceID == 1){
			tex_coord.x += float((fps/10) % 10) * 16.f/128.f;
		}
		if (gl_InstanceID == 2){
			tex_coord.x += float( (fps/100) % 10) * 16.f/128.f;
		}
		tex_coord.y +=1.f;
		tex_coord.y *=.5f;
//		tex_coord.x *= 64.f/128.f;
//		tex_coord.x += 12.f/16.f;//60.f/128.f;
//		tex_coord.x += float(gl_InstanceID) * 8.f/128.f 
}
