#include "main.h"


Mesh::Mesh(int ID){
  // textures = textures;
  CreateMeshVBO();
}

void Mesh::CreateMeshVBO(){

   glGenBuffers(2, GBO);
   GLuint vertexdat = GBO[0];
   GLuint veridxdat = GBO[1];
   glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
   glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), cube_vertices, GL_STATIC_DRAW);
   // used in glDrawElement()
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLuint), cube_indices, GL_STATIC_DRAW);
   std::cout<<"after GBO created"<<std::endl;
   glGenVertexArrays(1, &VAO);
   // vao like a closure binding 3 buffer object: verlocdat vercoldat and veridxdat
   glBindVertexArray(VAO);
   glEnableVertexAttribArray(0); // for vertexloc
   // glEnableVertexAttribArray(1); // for vertexcol
   std::cout<<"after VAO created"<<std::endl;
   // the vertex location is the same as the vertex color
   glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat *)NULL);
   // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat *)NULL);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
   std::cout<<"after bind buffer"<<std::endl;
}

void Mesh::Bind(){
  glBindVertexArray(VAO);
}

void Mesh::Draw(){
  //This don't work
  // draw mesh
  glEnable(GL_CULL_FACE);
  // glCullFace(glFaces);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)NULL);
  glDisable(GL_CULL_FACE);
}
