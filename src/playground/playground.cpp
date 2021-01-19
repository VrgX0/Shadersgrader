#include "playground.h"

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include chrono for sleep command
#include <chrono>
#include <thread>

#include <common/shader.hpp>

int main( void )
{
  //Initialize window
  bool windowInitialized = initializeWindow();
  if (!windowInitialized) return -1;

  //Initialize vertex buffer
  bool vertexbufferInitialized = initializeVertexbuffer();
  if (!vertexbufferInitialized) return -1;

  // Create and compile our GLSL program from the shaders
  programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

  // Get a handle for our "MVP" and "MV" uniforms and update them for initalization 
  MatrixIDMV = glGetUniformLocation(programID, "MV");
  MatrixID = glGetUniformLocation(programID, "MVP");
  updateMVPTransformation();

  //initialize pose variables
  curr_x = 1;
  curr_y = 1;
  curr_angle = 40;

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

	//start animation loop until escape key is pressed
	do{

    updateAnimationLoop();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

  //Cleanup and close window
  cleanupVertexbuffer();
  glDeleteProgram(programID);
	closeWindow();
  
	return 0;
}

void updateAnimationLoop()
{
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use our shader
  glUseProgram(programID);

  // Update the variables for movement / rotation if a key was pressed
  if (glfwGetKey(window, GLFW_KEY_W)) curr_y += 1.0;
  else if (glfwGetKey(window, GLFW_KEY_S)) curr_y -= 1.0;
  else if (glfwGetKey(window, GLFW_KEY_A)) curr_x -= 1.0;
  else if (glfwGetKey(window, GLFW_KEY_D)) curr_x += 1.0;
  else if (glfwGetKey(window, GLFW_KEY_R)) roll = true;


  // Update the MVP transformation with the new values
  updateMVPTransformation();

  // Send our transformation to the currently bound shader, 
  // in the "MVP" uniform and also the "MV" uniform
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(MatrixIDMV, 1, GL_FALSE, &MV[0][0]);

  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // 2nd attribute buffer : normals
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
  glVertexAttribPointer(
    1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size); // 3 indices starting at 0 -> 1 triangle

  // Swap buffers
  glfwSwapBuffers(window);
  glfwPollEvents();
}

bool initializeWindow()
{
  // Initialise GLFW
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return false;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "Example: 3D Model with basic shading", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    getchar();
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return false;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  return true;
}

bool updateMVPTransformation()
{
  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f);

  // Camera matrix (modify this to let the camera move)
  glm::mat4 View = glm::lookAt(
    glm::vec3(0, 0, 300), // Camera is at (0,0,0), in World Space
    glm::vec3(0, 0, 0), // and looks at the origin
    glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );

  // Model matrix, example with 3 parameters (curr_angle, curr_x, curr_y)
  glm::mat4 Model = glm::mat4(1.0f); //start with identity matrix
  
  

  srand((unsigned)time(0));
  int randomNumber = (rand() % 1000000000000000) + 1;
  if (roll) {
      float temp = .05f;
      for (int index = 0; index < randomNumber; index++) {
          timeValue = glfwGetTime();
          Model = glm::rotate(Model, timeValue/2, glm::vec3(1.0f, 0.0f, 0.0f));
          Model = glm::rotate(Model, timeValue/2, glm::vec3(0.0f, 1.0f, 0.0f));
          Model = glm::rotate(Model, timeValue/2, glm::vec3(0.0f, 0.0f, 1.0f));
          Model = glm::translate(Model, glm::vec3(0.0f, temp, 0.0f)); //apply translation
          temp -= 0.01f;
          temp *= -1;
      }
      roll = false;
  }
   
  
  Model = glm::rotate(Model, timeValue, glm::vec3(1.0f, 0.0f, 0.0f)); //apply orientation (last parameter: axis)
 // Model = glm::translate(Model,glm::vec3(curr_x, curr_y, 0.0f)); //apply translation
  Model = glm::scale(Model,glm::vec3(3.0f, 3.0f, 3.0f));
  
  // Our ModelViewProjection : multiplication of our 3 matrices
  MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
  MV = View * Model; //We also need MV in the shader to transform the light position

  return true;
}


bool initializeVertexbuffer()
{
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  //create vertex and normal data
  std::vector< glm::vec3 > vertices = std::vector< glm::vec3 >();
  std::vector< glm::vec3 > normals = std::vector< glm::vec3 >();
  loadSTLFile(vertices, normals, "d6_new.stl");
  vertexbuffer_size = vertices.size() * sizeof(glm::vec3);
  
  glGenBuffers(2, vertexbuffer); //generate two buffers, one for the vertices, one for the normals
  
  //fill first buffer (vertices)
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //fill second buffer (normals)
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  return true;
}

bool cleanupVertexbuffer()
{
  // Cleanup VBO
  glDeleteVertexArrays(1, &VertexArrayID);
  return true;
}

bool closeWindow()
{
  glfwTerminate();
  return true;
}

void loadSTLFile(std::vector< glm::vec3 >& vertices, 
                 std::vector< glm::vec3 >& normals, 
                 std::string stl_file_name)
{
  stl::stl_data info = stl::parse_stl(stl_file_name);
  std::vector<stl::triangle> triangles = info.triangles;
  for (int i = 0; i < info.triangles.size(); i++) {
    stl::triangle t = info.triangles.at(i);
    glm::vec3 triangleNormal = glm::vec3(t.normal.x, 
                                         t.normal.y, 
                                         t.normal.z);
    //add vertex and normal for point 1:
    vertices.push_back(glm::vec3(t.v1.x, t.v1.y, t.v1.z));
    normals.push_back(triangleNormal);
    //add vertex and normal for point 2:
    vertices.push_back(glm::vec3(t.v2.x, t.v2.y, t.v2.z));
    normals.push_back(triangleNormal);
    //add vertex and normal for point 3:
    vertices.push_back(glm::vec3(t.v3.x, t.v3.y, t.v3.z));
    normals.push_back(triangleNormal);
  }
}
