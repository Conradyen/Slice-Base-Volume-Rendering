#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include "main.h"

Camera::Camera(glm::vec3 position_ ,
glm::vec3 front_ ,
glm::vec3 up_ ,
glm::vec3 right_ ,
float zoom_ ,
float near_,
float far_ ,
unsigned int width_ ,
unsigned int height_ )
{
  this->ori_position = position_;
  this->ori_front = front_;
  this->ori_up = up_;
  this->ori_right = right_;
  this->ori_zoom = zoom_;
  this->near = near_;
  this->far = far_;
  this->width = width_;
  this->height = height_;
}

// Constructor with vectors
void Camera::init() {
reset();
}

void Camera::reset() {
this->position = ori_position;

this->front = ori_front;
this->up = ori_up;
this->right = ori_right;
this->zoom = ori_zoom;
}

// view getter
glm::mat4 Camera::get_view_mat(){
this->view_mat = glm::lookAt(this->position, glm::vec3(0.0f, 0.0f, 0.0f), this->up);
return this->view_mat;
}

glm::mat4 Camera::get_projection_mat(){
this->proj_mat = glm::perspective(glm::radians(this->zoom), (GLfloat)this->width / (GLfloat)this->height, this->near, this->far);
return this->proj_mat;
}

glm::vec3 Camera::GetViewDir(){
  return this->front;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::HandelKeyboardInput(Camera_Movement direction,  GLfloat delta_time){
  GLfloat move_velocity = delta_time * 10;
  GLfloat rotate_velocity = delta_time * 50;

  if (direction == FORWARD)
    this->position += this->front * move_velocity;
  if (direction == BACKWARD)
    this->position -= this->front * move_velocity;
  if (direction == LEFT)
    this->position -= this->right * move_velocity;
  if (direction == RIGHT)
    this->position += this->right * move_velocity;
  if (direction == UP)
    this->position += this->up * move_velocity;
  if (direction == DOWN)
    this->position -= this->up * move_velocity;
  if (direction == ROTATE_X_UP)
    rotate_x(rotate_velocity);
  if (direction == ROTATE_X_DOWN)
    rotate_x(-rotate_velocity);
  if (direction == ROTATE_Y_UP)
    rotate_y(rotate_velocity);
  if (direction == ROTATE_Y_DOWN)
    rotate_y(-rotate_velocity);
  if (direction == ROTATE_Z_UP)
    rotate_z(rotate_velocity);
  if (direction == ROTATE_Z_DOWN)
    rotate_z(-rotate_velocity);
}
// Rotate specific angle along local camera system(LCS)
void Camera::rotate_x(GLfloat angle){
  glm::vec3 up = this->up;
  glm::mat4 rotation_mat(1);
  rotation_mat = glm::rotate(rotation_mat, glm::radians(angle), this->right);
  this->up = glm::normalize(glm::vec3(rotation_mat * glm::vec4(up, 1.0)));
  this->front = glm::normalize(glm::cross(this->up, this->right));
}

void Camera::rotate_y(GLfloat angle){
  glm::vec3 front = this->front;
  glm::mat4 rotation_mat(1);
  rotation_mat = glm::rotate(rotation_mat, glm::radians(angle), this->up);
  this->front = glm::normalize(glm::vec3(rotation_mat * glm::vec4(front, 1.0)));
  this->right = glm::normalize(glm::cross(this->front, this->up));
}

void Camera::rotate_z(GLfloat angle){
  glm::vec3 right = this->right;
  glm::mat4 rotation_mat(1);
  rotation_mat = glm::rotate(rotation_mat, glm::radians(angle), this->front);
  this->right = glm::normalize(glm::vec3(rotation_mat * glm::vec4(right, 1.0)));
  this->up = glm::normalize(glm::cross(this->right, this->front));
}
