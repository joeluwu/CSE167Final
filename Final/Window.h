#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Node.h"
#include "MatrixTransform.cpp"
#include "Cylinder.cpp"
#include "Pod.cpp"
#include "Cube.h"
#include "shader.h"
#include "OBJObject.h"

using namespace std;

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P;
	static glm::mat4 V;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    static void mouse_callback(GLFWwindow* window, int key, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static glm::vec3 trackBallMapping(double, double);
    static glm::mat4 getMatrix(glm::vec3, glm::vec3, float, int);

    static MatrixTransform* build_pod(float);         // 1 pod + 1 small arm
    static MatrixTransform* build_flower();      // 3 x (pod+arm)
    static MatrixTransform* build_arm(float);
    static MatrixTransform* build_level(float rot_angle, float height_offset);
    static MatrixTransform* build_center();
};

#endif
