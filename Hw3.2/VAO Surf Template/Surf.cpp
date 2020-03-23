#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> //for pi
#include "Surf.h"

static int N = 50;

//The surface to draw.
glm::vec3 surf(int i, int j)
{
   const float center = 0.5f*N;
   const float xy_scale = 20.0f / N;
   const float z_scale = 10.0f;

   float x = xy_scale * (i - center);
   float y = xy_scale * (j - center);

   float r = sqrt(x*x + y * y);
   float z = 1.0f;

   if (r != 0.0f)
   {
      z = sin(r) / r;
   }
   z = z * z_scale;

   return 0.05f*glm::vec3(x, y, z);
}

//The demo shape being currently draw.
glm::vec3 circle(int i)
{
   const float r = 1.0f;
   float theta = i * 2.0f*glm::pi<float>() / N;
   return glm::vec3(r*cos(theta), r*sin(theta), 0.0f);
}

GLuint create_surf_vbo()
{
   //Declare a vector to hold N vertices
   std::vector<glm::vec3> surf_verts(N);

   for(int i=0; i<N; i++)
   {
      surf_verts[i] = circle(i);
   }

   GLuint vbo;
   glGenBuffers(1, &vbo); //Generate vbo to hold vertex attributes for triangle.

   glBindBuffer(GL_ARRAY_BUFFER, vbo); //Specify the buffer where vertex attribute data is stored.
   
   //Upload from main memory to gpu memory.
   glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*surf_verts.size(), surf_verts.data(), GL_DYNAMIC_DRAW);

   return vbo;
}

GLuint create_surf_vao()
{
   GLuint vao;

   //Generate vao id to hold the mapping from attrib variables in shader to memory locations in vbo
   glGenVertexArrays(1, &vao);

   //Binding vao means that bindbuffer, enablevertexattribarray and vertexattribpointer state will be remembered by vao
   glBindVertexArray(vao);

   GLuint vbo = create_surf_vbo();

   const GLint pos_loc = 0; //See also InitShader.cpp line 164.

   glEnableVertexAttribArray(pos_loc); //Enable the position attribute.

   //Tell opengl how to get the attribute values out of the vbo (stride and offset).
   glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, 3*sizeof(float), 0);
   glBindVertexArray(0); //unbind the vao

   return vao;
}

void draw_surf(GLuint vao)
{
   glDrawArrays(GL_LINE_LOOP, 0, N);
}
