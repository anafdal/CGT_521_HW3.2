#version 400

uniform vec4 La = vec4(0.7);//light ambient color
uniform vec4 Ld = vec4(0.7);//light diffuse color
uniform vec4 Ls = vec4(1.0);//light spec color
//uniform vec4 ks = vec4(0.5);

uniform vec4 Ka;//Ambient Material Color
uniform vec4 Kd;//Diffuse Material Color
uniform vec4 Ks;//Spec Material Color

uniform float m;
uniform float index;//index of refraction
const float PI = 3.1415926535897932384626433832795;
//uniform vec3 F0;//index of refraction

//uniform vec3 color;

uniform float alpha = 20.0; //phong specular exponent (shininess)

const vec3 l = vec3(0.1, 0.707, 0.707); //world space (directional light direction)

uniform sampler2D texture;
uniform samplerCube cubemap; //not used yet
uniform float slider;
uniform float time;

uniform int choice;

out vec4 fragcolor;           
in vec2 tex_coord;

in vec3 normal;   //World-space normal vector
in vec3 p;        //World-space fragment position
in vec3 eye;      //World-space eye position
     
void main(void)//Phong
{   
   vec3 n = normalize(normal); // unit normal vector,n
   vec3 v = normalize(eye-p); // unit view vector,v
   vec3 r = reflect(-l, n); // unit reflection vector,r
   vec3 h=((l+v)/normalize(l+v));//unit half vector
  
  float F0=pow(((1-index)/(1+index)),2);//F initial
  float F=F0+(1-F0)*pow((1-max(dot(n,v),0.0001)),5);//F

  float D0=((1-pow((max(dot(n,h),0.0001)),2))/(pow((max(dot(n,h),0.0001)),2)));//precompute clamp values wih epsilon
  float D1=pow((max(dot(n,h),0.0001)),4);
  float D=(exp(-(D0/pow(m,2)))/((pow(m,2))*D1));//remove 4 so it seems metallic
  
  float G0=1;//1

  float G1_1=2*((max(dot(n,h),0.001)));
  float G1_2=(max(dot(v,h),0.001));//clamp this one 

  float G2=(G1_1*(dot(n,v))/G1_2);//2
  float G3=((G1_1*max(dot(n,l), 0.001))/G1_2);//3
  float G=min(min(G3,G2),G0);



   vec4 tex_color = texture2D(texture, tex_coord); //Using this as ka and kd
   if(choice==0){
   
        vec4 amb =La*Ka;
        vec4 diff = Ld*Kd;
        vec4 spec = Ks*Ls;

     fragcolor =( amb + diff*max(dot(n,l),0.0) + (spec*((F*G*D))/((max(dot(n,v),0.0001)*PI))));
     // fragcolor=La*Ka;
   }
   else if(choice==1){
   
    //fragcolor = vec4(F,1.0);
    fragcolor = vec4(F);

   }
   else if(choice==2){

    fragcolor =vec4(D);

   }
   else if(choice==3){
    //fragcolor = G*tex_color;
    fragcolor =vec4 (G);
   }
   else if(choice==4){
        //compute phong lighting in world space
        vec4 amb = tex_color*La;//Ia
        vec4 diff = tex_color*Ld*max(dot(n,l), 0.0);//Id
        vec4 spec = Ks*Ls*pow(max(dot(r,v), 0.0), alpha);//Is

        fragcolor =  amb + diff + spec;
   }

}




















