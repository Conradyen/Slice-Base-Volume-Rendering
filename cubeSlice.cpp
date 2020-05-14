#include "main.h"
#include <math.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/rotate_vector.hpp>

cubeSlice::cubeSlice(glm::vec3 viewDirection){
  std::cout<<"init cubeSLice"<<std::endl;
  numberOfSlice = 10;
  viewDir = viewDirection;
  glm::mat4 Model = glm::mat4(1.0f);
  Model = glm::rotate(Model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  for(int i = 0;i< 8;i++){
    cube_vertices[i] = glm::vec3(Model * glm::vec4(cube_vertices[i],1.0f));
  }
}

void cubeSlice::setNumSlice(int numSlices){
  numberOfSlice = numSlices;
}

void cubeSlice::getNewSlices(std::vector<std::vector<glm::vec3> > &slices,std::vector<int> lengthArr){
  std::cout<<"getNewSlices"<<std::endl;
  getZMinMax();
  calculateIntersect();
  for(int i = 0;i < numberOfSlice;i++){
    std::vector<glm::vec3> v;
    SlicePositionArray[i].getSortedVertex(v);
    slices.push_back(v);
    std::cout<<"v size "<<v.size()<<std::endl;
    lengthArr.push_back(v.size());
  }
}

void cubeSlice::getZMinMax(){
  //get the max and min distance of each vertex of the unit cube
	//in the viewing direction
	float max_z = glm::dot(viewDir, cube_vertices[0]);
	float min_z = max_dist;
	int max_index = 0;

	for(int i=1;i<8;i++) {
		//get the distance between the current unit cube vertex and
		//the view vector by dot product
		float dist = glm::dot(viewDir, cube_vertices[i]);

		//if distance is > max_dist, store the value and index
		if(dist > max_dist) {
			max_z = dist;
			max_index = i;
		}

		//if distance is < min_dist, store the value
		if(dist<min_dist)
			min_z = dist;
	}

	//expand it a little bit
	min_z -= EPSILON;
	max_z += EPSILON;


  daltaZ = (max_z-min_z)/float(numberOfSlice);
  std::cout<<"daltaZ "<<daltaZ<<std::endl;
}


void cubeSlice::linePlaneIntersection() {

	float denom = 0;

	//set the minimum distance as the plane_dist
	//subtract the max and min distances and divide by the
	//total number of slices to get the plane increment
	float plane_dist = min_dist;
	float plane_dist_inc = (max_dist-min_dist)/float(num_slices);

	//for all edges
	for(int i=0;i<12;i++) {
		//get the start position vertex by table lookup
		edgeStart[i] = vertexList[edges[edgeList[max_index][i]][0]];

		//get the direction by table lookup
		edgeDir[i] = vertexList[edges[edgeList[max_index][i]][1]]-edgeStart[i];

		//do a dot of edgeDir with the view direction vector
		denom = glm::dot(edgeDir[i], viewDir);

		//determine the plane intersection parameter (lambda) and
		//plane intersection parameter increment (lambda_inc)
		if (1.0 + denom != 1.0) {
			lambda_inc[i] =  plane_dist_inc/denom;
			lambda[i]     = (plane_dist - glm::dot(edgeStart[i],viewDir))/denom;
		} else {
			lambda[i]     = -1.0;
			lambda_inc[i] =  0.0;
		}
	}

}

void cubeSlice::calculateIntersect(){
  //generate slices starting from min_z
  // float slicePoint = min_z;
  // while(slicePoint <= max_z){
  //   TextureSlice s(0);
  //   for(int i = 0;i < 24;i+=2){
  //     glm::vec3 intersect;
  //     // std::cout<<i<<std::endl;
  //     if(linePlaneIntersection(intersect,cube_edges[i],cube_edges[i+1],glm::vec3(0.0f,0.0f,slicePoint))){
  //       std::cout<<"x "<<intersect.x<<" y "<<intersect.y<<" z "<<intersect.z<<std::endl;
  //       s.add(intersect);
  //     }
  //   }
  //   SlicePositionArray.push_back(s);
  //   slicePoint+=daltaZ;
  // }

  // glm::vec3 intersection[6];
	float dL[12];

	//loop through all slices
	for(int i=num_slices-1;i>=0;i--) {

		//determine the lambda value for all edges
		for(int e = 0; e < 12; e++)
		{
			dL[e] = lambda[e] + i*lambda_inc[e];
		}
    TextureSlice s(0);

    //if the values are between 0-1, we have an intersection at the current edge
		//repeat the same for all 12 edges
		if  ((dL[0] >= 0.0) && (dL[0] < 1.0))	{
			intersection[0] = edgeStart[0] + dL[0]*edgeDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0))	{
			intersection[0] = edgeStart[1] + dL[1]*edgeDir[1];
		}
		else if ((dL[3] >= 0.0) && (dL[3] < 1.0))	{
			intersection[0] = edgeStart[3] + dL[3]*edgeDir[3];
		}
		else continue;

		if ((dL[2] >= 0.0) && (dL[2] < 1.0)){
			intersection[1] = edgeStart[2] + dL[2]*edgeDir[2];
		}
		else if ((dL[0] >= 0.0) && (dL[0] < 1.0)){
			intersection[1] = edgeStart[0] + dL[0]*edgeDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0)){
			intersection[1] = edgeStart[1] + dL[1]*edgeDir[1];
		} else {
			intersection[1] = edgeStart[3] + dL[3]*edgeDir[3];
		}

		if  ((dL[4] >= 0.0) && (dL[4] < 1.0)){
			intersection[2] = edgeStart[4] + dL[4]*edgeDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)){
			intersection[2] = edgeStart[5] + dL[5]*edgeDir[5];
		} else {
			intersection[2] = edgeStart[7] + dL[7]*edgeDir[7];
		}
		if	((dL[6] >= 0.0) && (dL[6] < 1.0)){
			intersection[3] = edgeStart[6] + dL[6]*edgeDir[6];
		}
		else if ((dL[4] >= 0.0) && (dL[4] < 1.0)){
			intersection[3] = edgeStart[4] + dL[4]*edgeDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)){
			intersection[3] = edgeStart[5] + dL[5]*edgeDir[5];
		} else {
			intersection[3] = edgeStart[7] + dL[7]*edgeDir[7];
		}
		if	((dL[8] >= 0.0) && (dL[8] < 1.0)){
			intersection[4] = edgeStart[8] + dL[8]*edgeDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)){
			intersection[4] = edgeStart[9] + dL[9]*edgeDir[9];
		} else {
			intersection[4] = edgeStart[11] + dL[11]*edgeDir[11];
		}

		if ((dL[10]>= 0.0) && (dL[10]< 1.0)){
			intersection[5] = edgeStart[10] + dL[10]*edgeDir[10];
		}
		else if ((dL[8] >= 0.0) && (dL[8] < 1.0)){
			intersection[5] = edgeStart[8] + dL[8]*edgeDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)){
			intersection[5] = edgeStart[9] + dL[9]*edgeDir[9];
		} else {
			intersection[5] = edgeStart[11] + dL[11]*edgeDir[11];
		}

		//after all 6 possible intersection vertices are obtained,
		//we calculated the proper polygon indices by using indices of a triangular fan
		int indices[]={0,1,2, 0,2,3, 0,3,4, 0,4,5};

		//Using the indices, pass the intersection vertices to the vTextureSlices vector
		for(int i=0;i<12;i++)
			vTextureSlices[count++]=intersection[indices[i]];
	}
}

// void cubeSlice::updateSlice(glm::vec3 viewDirection,int numSlices,std::vector<glm::vec3> &vTextureSlices){
//   numberOfSlice = numSlices;
//   viewDir = viewDirection;
//   getZMinMax();
//   linePlaneIntersection();
//   calculatePoint(vTextureSlices);
// }


int cubeSlice::getNumSlice(){
  return count;
}
