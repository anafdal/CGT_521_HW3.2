#version 400

out vec4 fragcolor;           
in vec3 pos;

const vec4 color1 = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 color2 = vec4(0.4, 0.4, 0.4, 1.0);
     
void main(void)
{   
   vec3 npos = normalize(pos);
   fragcolor = mix(color2, color1, npos.y+1.0);
}




















