#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
//GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GL/glew.h>
#include "stb_image/stb_image.h"
//applicaiton file
#include "main.h"


GLubyte * load_3d_raw_data(std::string texture_path, glm::vec3 dimension) {
  /**
   * Load .raw file for volume rendering
   * @param texture_path path to texture file
   * @param dimension 3D dimention of texture
   */
	size_t size = dimension.x * dimension.y * dimension.z;
	std::cout << texture_path << std::endl;
	FILE *fp;
	GLubyte *data = new GLubyte[size];			  // 8bit
	if (!(fp = fopen(texture_path.c_str(), "rb"))) {
		std::cout << "Error: opening .raw file failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "OK: open .raw file successed" << std::endl;
	}
	if (fread(data, sizeof(char), size, fp) != size) {
		std::cout << "Error: read .raw file failed" << std::endl;
		exit(1);
	}
	else {
		std::cout << "OK: read .raw file successed" << std::endl;
	}
	fclose(fp);
	return data;
}

void load1DTexturecolorBar(unsigned int textureID){

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, textureID);

	int width, height, nrComponents;

	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("data_and_code/colorbar.png", &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		// set the texture parameters
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//allocate the data to texture memory. Since pData is on stack, we donot delete it
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 3 * sizeof(float) * 4, GL_RGBA, GL_FLOAT, data);
		float pdata = data[256];
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << std::endl;
		stbi_image_free(data);
	}
}

CenteredView getMinMax(std::vector<Vertex> &vertices){
  GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
  min_x = max_x = vertices[0].Position.x;
  min_y = max_y = vertices[0].Position.y;
  min_z = max_z = vertices[0].Position.z;
  for (int i = 0; i < vertices.size(); i++) {
    if (vertices[i].Position.x < min_x) min_x = vertices[i].Position.x;
    if (vertices[i].Position.x > max_x) max_x = vertices[i].Position.x;
    if (vertices[i].Position.y < min_y) min_y = vertices[i].Position.y;
    if (vertices[i].Position.y > max_y) max_y = vertices[i].Position.y;
    if (vertices[i].Position.z < min_z) min_z = vertices[i].Position.z;
    if (vertices[i].Position.z > max_z) max_z = vertices[i].Position.z;
  }
  CenteredView cv;
  glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
  glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, 0.0f);
  cv.transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
  GLfloat max_edge = 0.0f;
  if(((max_x-min_x)) > max_edge){
    max_edge = ((max_x-min_x));
  }if(((max_y-min_y)) > max_edge){
    max_edge = ((max_y-min_y));
  }
  cv.cameracenter = glm::vec3((min_x+max_x)/2,(min_y+max_y)/2,max_z+(max_edge/tan(glm::radians(45.f / 2.f))));
  return cv;
}

CenteredView getCubeMinMax(){
	//only for cube mesh on voume rendering cube center at (0.5,0.5,0.5)

  //GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
  GLfloat min_x = 0.0f;
	GLfloat max_x = 1.0f;
	GLfloat min_y = 0.0f;
	GLfloat max_y = 1.0f;
	GLfloat min_z = 0.0f;
	GLfloat max_z = 1.0f;
	std::cout<<"min max"<<std::endl;
  CenteredView cv;
  glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
  glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
  cv.transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
	std::cout<<"cv"<<std::endl;
	GLfloat max_edge = 0.0f;
  if(((max_x-min_x)) > max_edge){
    max_edge = ((max_x-min_x));
  }if(((max_y-min_y)) > max_edge){
    max_edge = ((max_y-min_y));
  }
	std::cout<<"max edge"<<std::endl;
  cv.cameracenter = glm::vec3((min_x+max_x)/2,(min_y+max_y)/2,max_z+(max_edge/tan(glm::radians(45.f / 2.f))));
  return cv;
}

GLubyte * LoadColorBar(std::string texture_path){
	GLubyte * m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	//3 for RGB
	m_LocalBuffer = stbi_load(texture_path.c_str(), &m_Width, &m_Height, &m_BPP, 3);
	//only return the first row for intensity mapping
	return m_LocalBuffer;
}
