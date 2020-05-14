#include <cmath>
#include "main.h"

TextureSlice::TextureSlice(int ID){
  std::cout<<"init textureSlice"<<std::endl;
  center = glm::vec3(0.0f,0.0f,0.0f);
}

void TextureSlice::add(glm::vec3 vertex){
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
