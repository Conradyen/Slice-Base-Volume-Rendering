#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <unordered_map>

//GLM
#include "glm/glm.hpp"
#include <GL/glew.h>

//structs

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    ROTATE_X_UP,
  	ROTATE_X_DOWN,
  	ROTATE_Y_UP,
  	ROTATE_Y_DOWN,
  	ROTATE_Z_UP,
  	ROTATE_Z_DOWN,
};

struct CenteredView {
// Position
glm::vec3 size;
// Normal
glm::vec3 center;
// TexCoords
glm::mat4 transform;
//camera position
glm::vec3 cameracenter;
};

struct angelV{
  glm::vec3 position;
  float angel;
};


struct Vertex {
// Position
 glm::vec3 Position;
// Normal
 glm::vec3 Normal;
// TexCoords
 glm::vec2 TexCoords;
 glm::vec3 Tangent;
 glm::vec3 Bitangent;
};

struct obj_index {

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coods;
};

// ============= constant =================================
// glm::vec3(0.0, 0.0, 0.0),
// glm::vec3(0.0, 0.0, 1.0),
// glm::vec3(0.0, 1.0, 0.0),
// glm::vec3(0.0, 1.0, 1.0),
// glm::vec3(1.0, 0.0, 0.0),
// glm::vec3(1.0, 0.0, 1.0),
// glm::vec3(1.0, 1.0, 0.0),
// glm::vec3(1.0, 1.0, 1.0)

// ============================================================

//utils
GLubyte * load_3d_raw_data(std::string texture_path, glm::vec3 dimension);
CenteredView getMinMax(std::vector<Vertex> &vertices);
CenteredView getCubeMinMax();
void load1DTexturecolorBar(unsigned int textureID);
//============================================
//view

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.5f;
const float ZOOM        =  45.0f;

class Camera
{
public:
    // Camera view parameters
    glm::vec3 ori_position;
    glm::vec3 ori_front;
    glm::vec3 ori_up;
    glm::vec3 ori_right;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    // Camera projection parameters
    float ori_zoom;
    float zoom;

    float near;
    float far;

    unsigned int width;
    unsigned int height;

    // Camera projection matrix: used for projection
    glm::mat4 proj_mat;

    // Camera view matrix: used for changing camera rotation and position
    glm::mat4 view_mat;

    // Camera parameter initialization
    Camera(glm::vec3 position_ ,
    glm::vec3 front_,
    glm::vec3 up_ ,
    glm::vec3 right_,
    float zoom_,
    float near_ ,
    float far_ ,
    unsigned int width_,
    unsigned int height_);

    void init();
    void reset();
    void HandelKeyboardInput(Camera_Movement direction, GLfloat delta_time);
    // Rotate specific angle along local camera system(LCS)
    void rotate_x(GLfloat angle);
    void rotate_y(GLfloat angle);
    void rotate_z(GLfloat angle);
    glm::vec3 GetViewDir();
    // Get camera view matrix
    glm::mat4 get_view_mat();
    // Get camera projection matrix
    glm::mat4 get_projection_mat();
};

//====================================================================


//Shader
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
    private:
        unsigned int m_RendererID;
        std::string m_FilePath;
        std::unordered_map<std::string, int> m_UniformLocationCache;

    public:
        Shader(const std::string& filepath);
        ~Shader();

        void Bind() const;
        void Unbind() const;
        void SetUniform3f(const std::string& name, float f0, float f1, float f2);
        void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);
        void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
        void SetUniformInt(const std::string& name, int i);
        void SetUniform1f(const std::string& name, float f);

    private:
        int GetUniformLocation(const std::string& name);
        struct ShaderProgramSource ParseShader(const std::string& filepath);
        unsigned int CompileShader(unsigned int type, const std::string& source);
        unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

};
//=============================================

//mesh


class Mesh {
    public:
        /*  Mesh Data  */

        //cubes coordinates for binding 3d texture
        GLfloat cube_vertices[24] = {
        	0.0, 0.0, 0.0,
        	0.0, 0.0, 1.0,
        	0.0, 1.0, 0.0,
        	0.0, 1.0, 1.0,
        	1.0, 0.0, 0.0,
        	1.0, 0.0, 1.0,
        	1.0, 1.0, 0.0,
        	1.0, 1.0, 1.0
        };

        GLuint cube_indices[36] = {
        	1,5,7,
        	7,3,1,
        	0,2,6,
        	6,4,0,
        	0,1,3,
        	3,2,0,
        	7,5,4,
        	4,6,7,
        	2,3,7,
        	7,6,2,
        	1,0,4,
        	4,5,1
        };

        GLuint cube_edges[24] = {
        	1,5,
        	5,7,
        	7,3,
        	3,1,
        	0,4,
        	4,6,
        	6,2,
        	2,0,
        	0,1,
        	2,3,
        	4,5,
        	6,7
        };

        //vector<Texture> textures;

        /*  Functions  */
        Mesh(int ID);
        void Draw();
        void Bind();

    private:
        /*  Render data  */
        unsigned int VAO, VBO, EBO;
        GLuint GBO[2];

        int len;
        /*  Functions    */
        void CreateMeshVBO();
};

//=================================================

//textures

class Texture
{
    private:
        unsigned int m_RendererID;
        std::string m_FilePath;
        GLubyte * m_LocalBuffer;

        // Glint g_volTexObj;
    public:
        Texture(const std::string& path,glm::vec3 dimension);
        ~Texture();

        void Bind(unsigned int slot = 0) const;
        void Unbind() const;

        // inline int GetWidth() const { return m_Width; }
        // inline int GetHeight() const { return m_Height; }
};


class RawObject{
public:
  RawObject(std::string file_path,glm::vec3 dimension);

  std::string file_Path;
  glm::vec3 dimension;
};

class TextureSlice{
public:
  glm::vec3 center;
  TextureSlice(int ID);
  void getSortedVertex(std::vector<glm::vec3> &vertices);
  void add(glm::vec3 vertex);
  static bool PseudoangleComparator(angelV V1,angelV V2){
  	return V1.angel < V2.angel;
  };
private:

  static float Pseudoangle(glm::vec3 p1, glm::vec3 p2);
  void ComputeCenter();
  std::vector<glm::vec3> sliceVertex;
  std::vector<angelV> angelVertex;
};

class cubeSlice{
public:
  cubeSlice(glm::vec3 viewDirection);
  void setNumSlice(int numSlices);
  // void updateSlice(glm::vec3 viewDirection,int numSlices,std::vector<glm::vec3> &vTextureSlices);
  void getNewSlices(std::vector<std::vector<glm::vec3> > &slices,std::vector<int> lengthArr);
  void getZMinMax();
  void linePlaneIntersection();
  void calculateIntersect();
  int getNumSlice();
  glm::vec3 cube_vertices[8] = {
    glm::vec3(-0.5,-0.5,-0.5),
   glm::vec3(0.5,-0.5,-0.5),
   glm::vec3(0.5, 0.5,-0.5),
   glm::vec3(-0.5, 0.5,-0.5),
   glm::vec3(-0.5,-0.5, 0.5),
   glm::vec3(0.5,-0.5, 0.5),
   glm::vec3(0.5, 0.5, 0.5),
   glm::vec3(-0.5, 0.5, 0.5)
  };

  GLuint cube_indices[36] = {
    1,5,7,
    7,3,1,
    0,2,6,
    6,4,0,
    0,1,3,
    3,2,0,
    7,5,4,
    4,6,7,
    2,3,7,
    7,6,2,
    1,0,4,
    4,5,1
  };

  GLuint cube_edges[24] = {
    1,5,
    5,7,
    7,3,
    3,1,
    0,4,
    4,6,
    6,2,
    2,0,
    0,1,
    2,3,
    4,5,
    6,7
  };

private:
  glm::vec3 edgeStart[12];
  glm::vec3 edgeDir[12];
  float lambda[12];
  float lambda_inc[12];
  float denom;
  glm::vec3 viewDir;
  int numberOfSlice;
  float max_x;
  float min_x;
  float max_y;
  float min_y;
  float max_z;
  float min_z;
  float daltaZ;
  int minZidx;
  int maxZidx;
  int count;
  const float EPSILON = 0.0001f;
  const int MAX_SLICES = 2000;
  // glm::vec3 vTextureSlices[numberOfSlice * 12];
  std::vector<TextureSlice> SlicePositionArray;
};
