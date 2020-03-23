#include <windows.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "InitShader.h"
#include "LoadMesh.h"
#include "LoadTexture.h"
#include "imgui_impl_glut.h"

#include "Cube.h"
#include "Surf.h"


//Texture files and IDs
static const std::string texture_name = "AmagoT.bmp";
GLuint texture_id = -1; //Texture map for fish

static const std::string cube_name = "cubemap";
GLuint cubemap_id = -1; //Texture id for cubemap

//Mesh files and IDs
static const std::string vertex_shader("mesh_vs.glsl");
static const std::string fragment_shader("mesh_fs.glsl");
GLuint mesh_shader_program = -1;
static const std::string mesh_name = "Amago0.obj";
MeshData mesh_data;

bool mesh_enabled = true;

//Surf files and IDs
static const std::string surf_vs("surf_vs.glsl");
static const std::string surf_fs("surf_fs.glsl");
GLuint surf_shader_program = -1;
GLuint surf_vao = -1;

//Cube files and IDs
static const std::string cube_vs("cube_vs.glsl");
static const std::string cube_fs("cube_fs.glsl");
GLuint cube_shader_program = -1;
GLuint cube_vao = -1;


//camera and viewport
float camangle = 0.0f;
glm::vec3 campos(0.0f, 1.0f, 2.0f);
float aspect = 1.0f;

//ka,kd,ks
glm::vec4 Ka(0.0f,0.0f,0.0f,0.0f);
glm::vec4 Kd(0.0f, 0.0f, 0.0f, 0.0f);
glm::vec4 Ks(0.5f, 0.5f, 0.5f, 0.5f);

//m and index of refraction
static float m = 1.0f;
static float index = 1.0f;
static int choice = 0;
static float col[3] = { 1.0f,1.0f,0.2f };///colorEDit

void draw_gui()
{
   glUseProgram(mesh_shader_program);
   static bool first_frame = true;
   ImGui_ImplGlut_NewFrame();

   ImGui::Begin("VAO Surf", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

   ImGui::Checkbox("Draw mesh", &mesh_enabled); ImGui::SameLine();
   ImGui::SliderFloat3("Cam Pos", &campos[0], -20.0f, +20.0f);
   ImGui::SliderFloat("Cam Angle", &camangle, -180.0f, +180.0f);
   

   //widgets for ka,kd,ks
  // ImGui::SliderFloat("Ambient Material Color Ka", &Ka,-1.0f,+1.0f);
  // ImGui::SliderFloat("Diffuse Material Color Kd", &Kd, -1.0f, +1.0f);
   //ImGui::SliderFloat("Specular Material Color Ks", &Ks, -1.0f, +1.0f);
   
   ImGui::SliderFloat4("Ambient Material Color", &Ka[0], -1.0, +1.0f);
   ImGui::SliderFloat4("Diffuse Material Color ", &Kd[0], -1.0, +1.0f);
   ImGui::SliderFloat4("Specular Material Color ", &Ks[0], -1.0, +1.0f);


   //widget for m and index of refraction
   ImGui::SliderFloat("m", &m, 0.1f, +5.0f);
   ImGui::SliderFloat("Index of refraction", &index, 0.1f, +5.0f);

       //F,D,G
  
   ImGui::RadioButton("Scene 0:Cook-Tarrance", &choice, 0);
   ImGui::RadioButton("Scene 1:F view", &choice, 1);
   ImGui::RadioButton("Scene 2:D view", &choice, 2);
   ImGui::RadioButton("Scene 3:G view", &choice, 3);
   ImGui::RadioButton("Scene 4:Phong", &choice, 4);


   
   ImGui::ColorEdit3("Color", col);
 
   ImGui::End();

   ImGui::Render();
   first_frame = false;
}

void draw_fish(const glm::mat4& P, const glm::mat4& V)
{
   glm::mat4 R = glm::rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
   glm::mat4 M = R*glm::scale(glm::vec3(2.0f*mesh_data.mScaleFactor));
   
   glUseProgram(mesh_shader_program);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_id);
   int tex_loc = glGetUniformLocation(mesh_shader_program, "texture");
   if (tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
   int cube_loc = glGetUniformLocation(mesh_shader_program, "cubemap");
   if (cube_loc != -1)
   {
      glUniform1i(cube_loc, 1); // we bound our texture to texture unit 1
   }

   int PVM_loc = glGetUniformLocation(mesh_shader_program, "PVM");
   if (PVM_loc != -1)
   {
      glm::mat4 PVM = P*V*M;
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   int M_loc = glGetUniformLocation(mesh_shader_program, "M");
   if (M_loc != -1)
   {
      glUniformMatrix4fv(M_loc, 1, false, glm::value_ptr(M));
   }

   int V_loc = glGetUniformLocation(mesh_shader_program, "V");
   if (V_loc != -1)
   {
      glUniformMatrix4fv(V_loc, 1, false, glm::value_ptr(V));
   }

   int choice_loc = glGetUniformLocation(mesh_shader_program, "choice");////////////////////////////////////////////////////////////////////////////////////choice
   if (choice_loc != -1)
   {
       glUniform1i(choice_loc, choice); 
   }
   int ka_loc = glGetUniformLocation(mesh_shader_program, "Ka");////////////////////////////////////////////////////////////////////////////////////ka
   if (ka_loc != -1)
   {
       glUniform4f(ka_loc,Ka[0],Ka[1],Ka[2],Ka[3]);
   }

   int kd_loc = glGetUniformLocation(mesh_shader_program, "Kd");////////////////////////////////////////////////////////////////////////////////////kd
   if (kd_loc != -1)
   {
       glUniform4f(kd_loc, Kd[0],Kd[1],Kd[2],Kd[3]);
   }
   int ks_loc = glGetUniformLocation(mesh_shader_program, "Ks");////////////////////////////////////////////////////////////////////////////////////ks
   if (ks_loc != -1)
   {
       glUniform4f(ks_loc,Ks[0], Ks[1], Ks[2], Ks[3]);
   }
  /* int col_loc = glGetUniformLocation(mesh_shader_program, "color");////////////////////////////////////////////////////////////////////////////////////ks
   if (col_loc != -1)
   {
       glUniform3f(col_loc, col[0], col[1], col[2]);
   }*/
   int m_loc = glGetUniformLocation(mesh_shader_program, "m");////////////////////////////////////////////////////////////////////////////////////m
   if (m_loc != -1)
   {
       glUniform1f(m_loc, m);
   }
   int index_loc = glGetUniformLocation(mesh_shader_program, "index");////////////////////////////////////////////////////////////////////////////////////ks
   if (index_loc != -1)
   {
       glUniform1f(index_loc, index);
   }

   glBindVertexArray(mesh_data.mVao);
   mesh_data.DrawMesh();
}

void draw_cube(const glm::mat4& P, const glm::mat4& V)
{
   glUseProgram(cube_shader_program);
   int PVM_loc = glGetUniformLocation(cube_shader_program, "PVM");
   if (PVM_loc != -1)
   {
      glm::mat4 Msky = glm::scale(glm::vec3(5.0f));
      glm::mat4 PVM = P*V*Msky;
      PVM[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }
   
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
   int cube_loc = glGetUniformLocation(cube_shader_program, "cubemap");
   if (cube_loc != -1)
   {
      glUniform1i(cube_loc, 1); // we bound our texture to texture unit 1
   }

   glDepthMask(GL_FALSE);
   glBindVertexArray(cube_vao);
   draw_cube(cube_vao);
   glDepthMask(GL_TRUE);
}

void draw_surf(const glm::mat4& P, const glm::mat4& V)
{
   glUseProgram(surf_shader_program);
   int PVM_loc = glGetUniformLocation(cube_shader_program, "PVM");
   if (PVM_loc != -1)
   {
      glm::mat4 R = glm::rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
      glm::mat4 M = R * glm::scale(glm::vec3(5.0f));
      glm::mat4 PVM = P*V*M;
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   glBindVertexArray(surf_vao);
   draw_surf(surf_vao);
}

// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the back buffer
   
   glm::mat4 V = glm::lookAt(campos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(camangle, glm::vec3(0.0f, 1.0f, 0.0f));
   glm::mat4 P = glm::perspective(80.0f, aspect, 0.1f, 100.0f); //not affine

   draw_cube(P, V);

   if(mesh_enabled)
   {
      draw_fish(P, V);
   }

   draw_surf(P, V);

   draw_gui();
   glutSwapBuffers();
}

void idle()
{
   glutPostRedisplay();

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   float time_sec = 0.001f*time_ms;

   glUseProgram(mesh_shader_program);
   int time_loc = glGetUniformLocation(mesh_shader_program, "time");
   if (time_loc != -1)
   {
      //double check that you are using glUniform1f
      glUniform1f(time_loc, time_sec);
   }
}

void printGlInfo()
{
   std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
   std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
   std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
   std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void initOpenGl()
{
   glewInit();

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_POINT_SPRITE);       // allows textured points
   glEnable(GL_PROGRAM_POINT_SIZE); //allows us to set point size in vertex shader
   glClearColor(0.65f, 0.65f, 0.65f, 1.0f);

   cubemap_id = LoadCube(cube_name);

   mesh_shader_program = InitShader(vertex_shader.c_str(), fragment_shader.c_str());
   //mesh and texture to be rendered
   mesh_data = LoadMesh(mesh_name);
   texture_id = LoadTexture(texture_name);

   cube_shader_program = InitShader(cube_vs.c_str(), cube_fs.c_str());
   cube_vao = create_cube_vao();

   surf_shader_program = InitShader(surf_vs.c_str(), surf_fs.c_str());
   surf_vao = create_surf_vao();

   ImGui_ImplGlut_Init(); // initialize the imgui system
}

// glut callbacks need to send keyboard and mouse events to imgui
void keyboard(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyCallback(key);
}

void keyboard_up(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
   ImGui_ImplGlut_PassiveMouseMotionCallback(x, y);
}

void special(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
   ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
   ImGui_ImplGlut_MouseButtonCallback(button, state);
}

void reshape(int w, int h)
{
   glViewport(0, 0, w, h);
   aspect = (float)w / h;
}

int main(int argc, char **argv)
{
   //Configure initial window state
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowPosition(5, 5);
   glutInitWindowSize(640, 640);
   int win = glutCreateWindow("VAO Surf");

   printGlInfo();

   //Register callback functions with glut. 
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(special);
   glutKeyboardUpFunc(keyboard_up);
   glutSpecialUpFunc(special_up);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(motion);
   glutIdleFunc(idle);
   glutReshapeFunc(reshape);

   initOpenGl();

   //Enter the glut event loop.
   glutMainLoop();
   glutDestroyWindow(win);
   return 0;
}