#version 400            
uniform mat4 PVM;
uniform mat4 M;
uniform mat4 V;

uniform float time;
uniform float slider;

in vec3 pos_attrib; //in object space
in vec2 tex_coord_attrib;
in vec3 normal_attrib; //in object space

out vec2 tex_coord;
out vec3 normal; 
out vec3 p; 
out vec3 eye;

const vec4 origin = vec4(0.0, 0.0, 0.0, 1.0); //w = 1 becase this is a point

void main(void)
{
   //Compute clip-space vertex position
   gl_Position = PVM*vec4(pos_attrib, 1.0);     //w = 1 becase this is a point

   tex_coord = tex_coord_attrib;

   //Compute world-space normal position
   normal = vec3(M*vec4(normal_attrib, 0.0));   //w = 0 becase this is a vector

   //Compute world-space vertex position
   p = vec3(M*vec4(pos_attrib, 1.0));           //w = 1 becase this is a point

   //Compute world-space eye position. Note, it would be much more efficient to pass V inverse as a uniform, or pass camera position directly
   eye = vec3(inverse(V)*origin);
}