#include <cmath>
#include "main.h"

TextureSlice::TextureSlice(int ID){
  /**
   * Texture slice computes all vertex from interaction with cube edges
   * @param center : center of the cube for 3-D texture
   */
  std::cout<<"init textureSlice"<<std::endl;
  center = glm::vec3(0.0f,0.0f,0.0f);
}

void TextureSlice::add(glm::vec3 vertex){
  /**
   * store interaction int sliceVertex
   * @param vertex interaction with edges on cube.
   */
  sliceVertex.push_back(vertex);
}
void TextureSlice::getSortedVertex(std::vector<glm::vec3> &vertices){
  std::cout<<"getSortedVertex"<<std::endl;
  ComputeCenter();
  if(angelVertex.size() > 3){
    std::sort(angelVertex.begin(),angelVertex.end(),PseudoangleComparator);
    int i = 0;
    int j = 1;
    while(i<angelVertex.size()){
      if(j == angelVertex.size()){
        j=0;
      }
      vertices.push_back(angelVertex[i].position);
      vertices.push_back(center);
      vertices.push_back(angelVertex[j].position);
      i++;j++;
    }
  }else{
    std::sort(angelVertex.begin(),angelVertex.end(),PseudoangleComparator);
    for(int i = 0;i< angelVertex.size() ;i++){
      vertices.push_back(angelVertex[i].position);
    }
  }
}

void TextureSlice::ComputeCenter(){
  /**
   * compute center of interaction of a slice sort interaction in counter clock
   * wise order.
   * ==========================================================================
   * example:
   * p1,p1 and center o
   *
   *               p1 *
   *
   *             o      p2 *
   *
   *     will push in vector in p1->p2->o order
   * ==========================================================================
   */
  float centerx = 0.0f;
  float centery = 0.0f;
  float centerz = 0.0f;
  int len = sliceVertex.size();
  for (int i = 0;i<len;i++){
    centerx += sliceVertex[i].x;
    centery += sliceVertex[i].y;
    centerz += sliceVertex[i].z;
  }
  center = glm::vec3(centerx/len,centery/len,centerz/len);
  for(int j = 0;j<len;j++){
    angelV v;
    v.position = sliceVertex[j];
    v.angel = Pseudoangle(center,sliceVertex[j]);
    angelVertex.push_back(v);
  }
}

float TextureSlice::Pseudoangle(glm::vec3 p1, glm::vec3 p2){
  /**
   * pseudo angle to sort vertex in counter clock wise order
   * @param p1 center of a slice
   * @param p2 slice interaction on cube edge
   */
  glm::vec3 delta = p2 - p1;
   float result;

   if ((delta.x == 0) && (delta.y == 0)) {
      return -1;
   } else {
      result = delta.y / (abs(delta.x) + abs(delta.y));

      if (delta.x < 0.0) {
         result = 2.0f - result;
      } else {
         result = 4.0f + result;
      }
   }
   return result;
}
