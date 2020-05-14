#define GLEW_STATIC
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <fstream>
#include <filesystem>
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
//application file
#include "main.h"
//GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/rotate_vector.hpp>
//imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <sstream>

glm::vec3 position_ = glm::vec3(0, 0, -3);
glm::vec3 front_ = glm::vec3(0, 0, -1);
glm::vec3 up_ = glm::vec3(0, 1, 0);
glm::vec3 right_ = glm::vec3(1, 0, 0);
float zoom_ = 0.0;
float near_ = 0.1;
float far_ = 500;

unsigned int window_width = 1280;
unsigned int window_height = 960;

int indices[] = { 0,1,2, 0,2,3, 0,3,4, 0,4,5 };

// camera
Camera camera(position_,front_,up_,right_,zoom_,near_,far_,window_width,window_height);

float lastX = (float)window_width / 2.0;
float lastY = (float)window_height / 2.0;
bool firstMouse = true;
// timing
float deltaFrame = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//static light
glm::vec3 lightPos( 0.0f, -1.0f, -1.0f );

//================ constant ======================================
RawObject Head("data_and_code/Head_256_256_225.raw",glm::vec3(256,256,225));
RawObject Bonsai("data_and_code/Bonsai_512_512_154.raw",glm::vec3(512,512,154));
RawObject BostonTeapot("data_and_code/BostonTeapot_256_256_178.raw",glm::vec3(256,256,178));
RawObject Bucky("data_and_code/Bucky_32_32_32.raw",glm::vec3(32,32,32));
//==================================================================

//unit cube vertices
glm::vec3 cube_vertices[8] = {glm::vec3(-0.5,-0.5,-0.5),
						   glm::vec3( 0.5,-0.5,-0.5),
						   glm::vec3(0.5, 0.5,-0.5),
						   glm::vec3(-0.5, 0.5,-0.5),
						   glm::vec3(-0.5,-0.5, 0.5),
						   glm::vec3(0.5,-0.5, 0.5),
						   glm::vec3( 0.5, 0.5, 0.5),
						   glm::vec3(-0.5, 0.5, 0.5)};

//unit cube edges
int edgeSequence[8][12] = {
	{ 0,1,5,6,   4,8,11,9,  3,7,2,10 },
	{ 0,4,3,11,  1,2,6,7,   5,9,8,10 },
	{ 1,5,0,8,   2,3,7,4,   6,10,9,11},
	{ 7,11,10,8, 2,6,1,9,   3,0,4,5  },
	{ 8,5,9,1,   11,10,7,6, 4,3,0,2  },
	{ 9,6,10,2,  8,11,4,7,  5,0,1,3  },
	{ 9,8,5,4,   6,1,2,0,   10,7,11,3},
	{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }
};
const int edges[12][2]= {{0,1},{1,2},{2,3},{3,0},{0,4},{1,5},{2,6},{3,7},{4,5},{5,6},{6,7},{7,4}};
const float EPSILON = 0.0001f;
//maximum number of slices
const int MAX_SLICES = 2000;

//sliced vertices
glm::vec3 renderSliceArray[MAX_SLICES*12];

//total number of slices current used
int num_slices = 10;
int count;
glm::vec3 viewDir = glm::vec3(0.0f,0.0f,-1.0f);
unsigned int VBO,VAO;

void GLFWHandelScroll(GLFWwindow* window, double x_offset,double y_offset);
void GLFWwindowSizeCallback(GLFWwindow* window,int height,int width);
void GLFWframebufferSizeCallback(GLFWwindow* window,int height,int width);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void initFrameBuffer(GLuint texObj, GLuint texWidth, GLuint texHeight);
GLuint initFace2DTex(GLuint bfTexWidth,GLuint frameBuffer, GLuint bfTexHeight);
void onModelChange(std::string modelName);

void onModelChange(std::string modelName,Shader shader){
  if(modelName == "Head"){
    Texture texture(Head.file_Path,Head.dimension);
    texture.Bind(0);
    shader.SetUniformInt("u_3DTexture", 0);
  }
  if(modelName == "Bonsai"){
    Texture texture(Bonsai.file_Path,Bonsai.dimension);
    texture.Bind(0);
    shader.SetUniformInt("u_3DTexture", 0);
  }
  if(modelName == "BostonTeapot"){
    Texture texture(BostonTeapot.file_Path,BostonTeapot.dimension);
    texture.Bind(0);
    shader.SetUniformInt("u_3DTexture", 0);
  }
  if(modelName == "Bucky"){
    Texture texture(Bucky.file_Path,Bucky.dimension);
    texture.Bind(0);
    shader.SetUniformInt("u_3DTexture", 0);
  }
}

void ViewSliceCube() {
	//reference from opengl33_dev_cookbook_2013

	float max_z = glm::dot(viewDir, cube_vertices[0]);
	float min_z = max_z;
	int max_index = 0;
	count = 0;

	for(int i=1;i<8;i++) {

		float dist = glm::dot(viewDir, cube_vertices[i]);
		if(dist > max_z) {
			max_z = dist;
			max_index = i;
		}
		if(dist<min_z)
			min_z = dist;
	}
	min_z -= EPSILON;
	max_z += EPSILON;

	glm::vec3 edgeStart[12];
	glm::vec3 edgeDir[12];
	float lambda[12];
	float lambda_inc[12];
	float edgedot = 0;
	float plane_dist = min_z;
	float plane_dist_inc = (max_z-min_z)/float(num_slices);

	//for all edges
	for(int i=0;i<12;i++) {
		//get the start position vertex by table lookup
		edgeStart[i] = cube_vertices[edges[edgeSequence[max_index][i]][0]];

		//get the direction by table lookup
		edgeDir[i] = cube_vertices[edges[edgeSequence[max_index][i]][1]]-edgeStart[i];

		edgedot = glm::dot(edgeDir[i], viewDir);

		if (1.0 + edgedot != 1.0) {
			lambda_inc[i] =  plane_dist_inc/edgedot;
			lambda[i]     = (plane_dist - glm::dot(edgeStart[i],viewDir))/edgedot;
		} else {
			lambda[i]     = -1.0;
			lambda_inc[i] =  0.0;
		}
	}
	glm::vec3 intersection[6];
	float daltaL[12];

	//loop through all slices
	for(int i=num_slices-1;i>=0;i--) {

		//determine the lambda value for all edges
		for(int e = 0; e < 12; e++)
		{
			daltaL[e] = lambda[e] + i*lambda_inc[e];
		}
		if  ((daltaL[0] >= 0.0) && (daltaL[0] < 1.0))	{
			intersection[0] = edgeStart[0] + daltaL[0]*edgeDir[0];
		}
		else if ((daltaL[1] >= 0.0) && (daltaL[1] < 1.0))	{
			intersection[0] = edgeStart[1] + daltaL[1]*edgeDir[1];
		}
		else if ((daltaL[3] >= 0.0) && (daltaL[3] < 1.0))	{
			intersection[0] = edgeStart[3] + daltaL[3]*edgeDir[3];
		}
		else continue;

		if ((daltaL[2] >= 0.0) && (daltaL[2] < 1.0)){
			intersection[1] = edgeStart[2] + daltaL[2]*edgeDir[2];
		}
		else if ((daltaL[0] >= 0.0) && (daltaL[0] < 1.0)){
			intersection[1] = edgeStart[0] + daltaL[0]*edgeDir[0];
		}
		else if ((daltaL[1] >= 0.0) && (daltaL[1] < 1.0)){
			intersection[1] = edgeStart[1] + daltaL[1]*edgeDir[1];
		} else {
			intersection[1] = edgeStart[3] + daltaL[3]*edgeDir[3];
		}

		if  ((daltaL[4] >= 0.0) && (daltaL[4] < 1.0)){
			intersection[2] = edgeStart[4] + daltaL[4]*edgeDir[4];
		}
		else if ((daltaL[5] >= 0.0) && (daltaL[5] < 1.0)){
			intersection[2] = edgeStart[5] + daltaL[5]*edgeDir[5];
		} else {
			intersection[2] = edgeStart[7] + daltaL[7]*edgeDir[7];
		}
		if	((daltaL[6] >= 0.0) && (daltaL[6] < 1.0)){
			intersection[3] = edgeStart[6] + daltaL[6]*edgeDir[6];
		}
		else if ((daltaL[4] >= 0.0) && (daltaL[4] < 1.0)){
			intersection[3] = edgeStart[4] + daltaL[4]*edgeDir[4];
		}
		else if ((daltaL[5] >= 0.0) && (daltaL[5] < 1.0)){
			intersection[3] = edgeStart[5] + daltaL[5]*edgeDir[5];
		} else {
			intersection[3] = edgeStart[7] + daltaL[7]*edgeDir[7];
		}
		if	((daltaL[8] >= 0.0) && (daltaL[8] < 1.0)){
			intersection[4] = edgeStart[8] + daltaL[8]*edgeDir[8];
		}
		else if ((daltaL[9] >= 0.0) && (daltaL[9] < 1.0)){
			intersection[4] = edgeStart[9] + daltaL[9]*edgeDir[9];
		} else {
			intersection[4] = edgeStart[11] + daltaL[11]*edgeDir[11];
		}

		if ((daltaL[10]>= 0.0) && (daltaL[10]< 1.0)){
			intersection[5] = edgeStart[10] + daltaL[10]*edgeDir[10];
		}
		else if ((daltaL[8] >= 0.0) && (daltaL[8] < 1.0)){
			intersection[5] = edgeStart[8] + daltaL[8]*edgeDir[8];
		}
		else if ((daltaL[9] >= 0.0) && (daltaL[9] < 1.0)){
			intersection[5] = edgeStart[9] + daltaL[9]*edgeDir[9];
		} else {
			intersection[5] = edgeStart[11] + daltaL[11]*edgeDir[11];
		}

		//after all 6 possible intersection vertices are obtained,
		int indices[]={0,1,2, 0,2,3, 0,3,4, 0,4,5};
		for(int i=0;i<12;i++)
			renderSliceArray[count++]=intersection[indices[i]];
	}

	//update buffer object with the new vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  sizeof(renderSliceArray), &(renderSliceArray[0].x));
}
GLFWwindow* InitWindow(){
    // Initialise GLFW
    if(!glfwInit()){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow( window_width, window_height, "assignment1", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return nullptr;

    }
    glfwMakeContextCurrent(window);
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(window, GLFWframebufferSizeCallback);
    std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;
    // glfwSetScrollCallback(window, GLFWHandelScroll);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    return window;
}

void initFrameBuffer(GLuint texObj,GLuint frameBuffer, GLuint texWidth, GLuint texHeight){
  // create a depth buffer for our framebuffer
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);

    // attach the texture and the depth buffer to the framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glEnable(GL_DEPTH_TEST);
}

// init the 2D texture for render backface 'bf' stands for backface
GLuint initFace2DTex(GLuint bfTexWidth, GLuint bfTexHeight)
{
    GLuint backFace2DTex;
    glGenTextures(1, &backFace2DTex);
    glBindTexture(GL_TEXTURE_2D, backFace2DTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bfTexWidth, bfTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    return backFace2DTex;
}

void GLFWwindowSizeCallback(GLFWwindow* window,int height,int width){
  glViewport(0, 0, width, height);
}

void GLFWframebufferSizeCallback(GLFWwindow* window,int height,int width){
  glViewport(0, 0, width, height);
}

void GLFWprocessKeyboardInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//close
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)//forward
        camera.HandelKeyboardInput(FORWARD, deltaFrame);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)//backward
        camera.HandelKeyboardInput(BACKWARD, deltaFrame);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)//left
        camera.HandelKeyboardInput(LEFT, deltaFrame);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)//right
        camera.HandelKeyboardInput(RIGHT, deltaFrame);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)//up
        camera.HandelKeyboardInput(UP, deltaFrame);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)//down
        camera.HandelKeyboardInput(DOWN, deltaFrame);
}



//=========================================================

int main(int argc, char *argv[]){

  if(argc < 2){
    std::cout<<"please enter file path\n";
  }
   const char* glsl_version = "#version 330";
   GLFWwindow* window = InitWindow();
    std::cout<<"window created\n";
    if (window == NULL)
        return -1;

    //
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    //shader
    Shader shader("shader/shader.code");
    shader.Bind();

		std::string fp = Head.file_Path;
		glm::vec3 di = Head.dimension;

		if(strcmp(argv[1],"Head")==0){
	    fp = Head.file_Path;
			di = Head.dimension;
	  }
	  else if(strcmp(argv[1] , "Bonsai")==0){
			fp = Bonsai.file_Path;
			di = Bonsai.dimension;
	  }
	  else if(strcmp(argv[1] , "BostonTeapot")==0){
			fp = BostonTeapot.file_Path;
			di = BostonTeapot.dimension;
	  }
	  else{
			fp = Bucky.file_Path;
			di = Bucky.dimension;
	  }
		Texture texture(fp,di);
		texture.Bind(0);
		shader.SetUniformInt("u_3DTexture", 0);

    // Texture texture(Bonsai.file_Path,Bonsai.dimension);
    // texture.Bind(0);
    // shader.SetUniformInt("u_3DTexture", 0);
    unsigned int ColorBartextureID;
    // load1DTexturecolorBar(ColorBartextureID);
    shader.SetUniformInt("barColor", 1);
    std::cout<<"before mesh created"<<std::endl;
    // Mesh cube(0);

      //===================== initialize imgui =====================
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO(); (void)io;
      ImGui_ImplGlfw_InitForOpenGL(window, true);
      ImGui_ImplOpenGL3_Init(glsl_version);
      ImGui::StyleColorsDark();
      float f0=0.123f, f1=0.023f,f2=0.045f,f3=0.05f,f4=0.7f,f5=0.3f,f6=0.023f,f7=0.03;
      float arr[8];
      arr[0] = f0;
      arr[1] = f1;
      arr[2] = f2;
      arr[3] = f3;
      arr[4] = f4;
      arr[5] = f5;
      arr[6] = f6;
      arr[7] = f7;
      float xyz_offset[] = {0.0f,0.0f,0.0f};
      float pre_offset_x = 0.0f;
      float pre_offset_y = 0.0f;
      float pre_offset_z = 0.0f;
      float cube_offset[] = {0.0f,0.0f,0.0f};
      float cube_pre_offset_x = 0.0f;
      float cube_pre_offset_y = 0.0f;
      float cube_pre_offset_z = 0.0f;

      float showslice = 1.0f;
      bool transferFunctionSwich = false;

      float alpha, a0, s = 10;
      float colorBarData[256][4];
	     int indices[9];
       //rander premitive
       const char* premitive[] = {"Head","Bonsai","BostonTeapot","Bucky"};
       static const char* curr_premitive = "Head";
      //============================================================

      //model view project matrix
      glm::mat4 Model, View, Projection,cModel;
      std::cout<<"view"<<std::endl;
      Model = glm::mat4(1.0f);
      cModel = glm::mat4(1.0f);
      Model = glm::rotate(Model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	    View = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	    Projection = glm::perspective(glm::radians(45.0f), ((float)window_width / (float)window_height), 0.1f, 200.0f);
      shader.Bind();
      shader.SetUniformMat4f("model",Model);
      shader.SetUniformMat4f("projection", Projection);
      shader.SetUniformMat4f("view", View);

      //=========== model view projection end ============================

      glGenVertexArrays(1, &VAO);
      // std::cout<<std::to_string(VAO)<<std::endl;
      glGenBuffers(1, &VBO);
      // glGenBuffers(1, &EBO);
      // std::cout<<std::to_string(renderVertex.size())<<std::endl;
      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(renderSliceArray), &renderSliceArray[0].x, GL_DYNAMIC_DRAW);

      // vertex positions
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
      ViewSliceCube();


      // std::cout<<"count"<<count<<std::endl;
      //get the viewing direction

      std::cout<<"before while loop"<<std::endl;
      while(!glfwWindowShouldClose(window)){

          //Timing
          float currentFrame = glfwGetTime();
          deltaFrame = currentFrame - lastFrame;
          lastFrame = currentFrame;
          //new frame
          glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

          //imgui new frame
          ImGui_ImplOpenGL3_NewFrame();
          ImGui_ImplGlfw_NewFrame();
          ImGui::NewFrame();

          //handel keyboard input
          GLFWprocessKeyboardInput(window);
          // glfwGetWindowSize(window, &window_width, &window_height);

          //control panel gui
          //GUI
          {//model control UI
            ImGui::Begin("Control Window");
            //take input
            // ImGui::InputText("input new file path", input_filePath, IM_ARRAYSIZE(input_filePath));
            // std::string s(input_filePath);

            ImGui::SliderFloat("X rotate", &xyz_offset[0], 0.0f, 180.0f, "%.1f");
            ImGui::SliderFloat("Y rotate", &xyz_offset[1], 0.0f, 180.0f, "%.1f");
            ImGui::SliderFloat("Z rotate", &xyz_offset[2], 0.0f, 180.0f, "%.1f");

            ImGui::SliderFloat("X rotate cube", &cube_offset[0], 0.0f, 359.0f, "%.1f");
            ImGui::SliderFloat("Y rotate cube", &cube_offset[1], 0.0f, 359.0f, "%.1f");
            ImGui::SliderFloat("Z rotate cube", &cube_offset[2], 0.0f, 359.0f, "%.1f");

            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            ImGui::End();
          }
          {//transfor function UI
           ImGui::Begin("transfer function");
           ImGui::Checkbox("ON/OFF transfer function",&transferFunctionSwich);
           ImGui::SliderInt("number of slice", &num_slices, 10, 2000, "%d");
           ImGui::SliderFloat("show slice", &showslice, 0.0f, 1.0f, "%.1f");
           ImVec2 plotextent(ImGui::GetContentRegionAvailWidth()*0.9,150);
           ImGui::Text("transfer function");
           ImGui::PlotLines(" ", &arr[0], 7, 0, NULL, 0.0f, FLT_MAX, plotextent, sizeof(float));
           ImGui::SliderFloat("slider 1", &arr[0], 0.0f, 1.0f, "%.3f");
           ImGui::SliderFloat("slider 2", &arr[1], 0.0f, 1.0f, "%.3f");
           ImGui::SliderFloat("slider 3", &arr[2], 0.0f, 1.0f, "%.3f");
           ImGui::SliderFloat("slider 4", &arr[3], 0.0f, 1.0f, "%.3f");
           ImGui::SliderFloat("slider 5", &arr[4], 0.0f, 1.0f, "%.3f");
           ImGui::SliderFloat("slider 6", &arr[5], 0.0f, 1.0f, "%.3f");
           ImGui::SliderFloat("slider 7", &arr[6], 0.0f, 1.0f, "%.3f");
            ImGui::End();
          }

          shader.Bind();
          if((xyz_offset[0] != pre_offset_x)){
            cModel = glm::rotate(cModel, glm::radians(pre_offset_x-xyz_offset[0]), glm::vec3(1.0f, 0.0f, 0.0f));
            pre_offset_x = xyz_offset[0];
            for(int i = 0;i< 8;i++){
              cube_vertices[i] = glm::vec3(cModel * glm::vec4(cube_vertices[i],1.0f));
            }
          }if((xyz_offset[1] != pre_offset_y)){
            cModel = glm::rotate(cModel, glm::radians(pre_offset_y-xyz_offset[1]), glm::vec3(0.0f, 1.0f, 0.0f));
            pre_offset_y = xyz_offset[1];
            for(int i = 0;i< 8;i++){
              cube_vertices[i] = glm::vec3(cModel * glm::vec4(cube_vertices[i],1.0f));
            }
          }if((xyz_offset[2] != pre_offset_z)){
            cModel = glm::rotate(cModel, glm::radians(pre_offset_z-xyz_offset[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            pre_offset_z = xyz_offset[2];
            for(int i = 0;i< 8;i++){
              cube_vertices[i] = glm::vec3(cModel * glm::vec4(cube_vertices[i],1.0f));
            }
          }
          if((cube_offset[0] != cube_pre_offset_x)){
            Model = glm::rotate(Model, glm::radians(cube_pre_offset_x-cube_offset[0]), glm::vec3(1.0f, 0.0f, 0.0f));
            cube_pre_offset_x = cube_offset[0];
          }if((cube_offset[1] != cube_pre_offset_y)){
            Model = glm::rotate(Model, glm::radians(cube_pre_offset_y-cube_offset[1]), glm::vec3(0.0f, 1.0f, 0.0f));
            cube_pre_offset_y = cube_offset[1];
          }if((cube_offset[2] != cube_pre_offset_z)){
            Model = glm::rotate(Model, glm::radians(cube_pre_offset_z-cube_offset[2]), glm::vec3(0.0f, 0.0f, 1.0f));
            cube_pre_offset_z = cube_offset[2];
          }
          shader.SetUniformMat4f("model",Model);
          shader.SetUniformInt("TransferFunction",transferFunctionSwich);



          alpha = 1 - (glm::pow((1 - 0.5), (s / num_slices)));

	for (int i = 0; i < 32; i++){
		colorBarData[i][0] = 0.0f;
		colorBarData[i][1] = 0.0f;
		colorBarData[i][2] = arr[0];
		colorBarData[i][3] = arr[0]*alpha;
	}
	for (int i = 32; i < 64; i++){
		colorBarData[i][0] = arr[1] * 0.5;
		colorBarData[i][1] = arr[1] * 0.5;
		colorBarData[i][2] = arr[1]  ;
		colorBarData[i][3] = arr[1] * alpha;
	}
	for (int i = 64; i < 96; i++){
		colorBarData[i][0] = arr[2] * 0.6;
		colorBarData[i][1] = arr[2] * 0.7;
		colorBarData[i][2] = arr[2] * 1.5;
		colorBarData[i][3] = alpha * arr[2];
	}
	for (int i = 96; i < 128; i++){
		colorBarData[i][0] = arr[3] * 0.8;
		colorBarData[i][1] = arr[3] * 0.8;
		colorBarData[i][2] = arr[3] * 1.0;
		colorBarData[i][3] = alpha * arr[3];
	}
	for (int i = 128; i < 160; i++){
		colorBarData[i][0] = arr[4];
		colorBarData[i][1] = arr[4] * 0.7;
		colorBarData[i][2] = arr[4] * 0.7;
		colorBarData[i][3] = alpha * arr[4];
	}
	for (int i = 160; i < 192; i++){
		colorBarData[i][0] = arr[5];
		colorBarData[i][1] = 0.2;
		colorBarData[i][2] =  0.1;
		colorBarData[i][3] = alpha * arr[5];
	}
	for (int i = 192; i < 224; i++){
		colorBarData[i][0] = arr[6];
		colorBarData[i][1] = 0.75* arr[6];
		colorBarData[i][2] = 0.75 * arr[6];
		colorBarData[i][3] = alpha * arr[6];
	}
	for (int i = 224; i < 256; i++){
		colorBarData[i][0] = arr[7];
		colorBarData[i][1] = 0.0f;
		colorBarData[i][2] = 0.0f;
		colorBarData[i][3] = alpha * arr[7];
	}

	glGenTextures(1, &ColorBartextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, ColorBartextureID);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, colorBarData);
          shader.SetUniformInt("barColor", 1);
          ViewSliceCube();
      	glBindVertexArray(VAO);

      	if (transferFunctionSwich){
      		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      	}
      	else{
      			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      	}

        glDrawArrays(GL_TRIANGLES, 0, (int)(floor(count/6)*showslice)*6);
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
      }

    // Close OpenGL window and terminate GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;

}
