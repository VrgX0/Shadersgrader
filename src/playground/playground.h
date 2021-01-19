#ifndef PLAYGROUND_H
#define PLAYGROUND_H

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>

#include <vector>
#include <playground/parse_stl.h>

//some global variables for handling the vertex buffer
GLuint vertexbuffer[2];
GLuint VertexArrayID;
GLuint vertexbuffer_size;

//program ID of the shaders, required for handling the shaders with OpenGL
GLuint programID;

//global variables to handle the MVP matrix
GLuint MatrixID;
glm::mat4 MVP;
GLuint MatrixIDMV;
glm::mat4 MV;

//global variables to handle the object pose
float curr_x;
float curr_y;
float curr_angle;
bool roll;
float timeValue;

int main( void ); //<<< main function, called at startup
void updateAnimationLoop(); //<<< updates the animation loop
bool initializeWindow(); //<<< initializes the window using GLFW and GLEW
bool updateMVPTransformation(); //<<< updates the MVP transform with the current pose
bool initializeVertexbuffer(); //<<< initializes the vertex buffer array and binds it OpenGL
bool cleanupVertexbuffer(); //<<< frees all recources from the vertex buffer
bool closeWindow(); //<<< Closes the OpenGL window and terminates GLFW

/** 
 * Loads a STL file and converts it to a vector of vertices and normals
 * @param[out] vertices   Vector of vertices, needs to be empty and is filled by the function.
 * @param[out] normals   Vector of normals, needs to be empty and is filled by the function.
   @param[in] stl_file_name File name of the STL file that should be loaded
 */
void loadSTLFile(std::vector< glm::vec3 >& vertices, std::vector< glm::vec3 >& normals, std::string stl_file_name);

#endif
