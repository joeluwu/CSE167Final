#include "window.h"

const char* window_title = "GLFW Starter Project";
glm::mat4 Window::P;
glm::mat4 Window::V;
int Window::width;
int Window::height;

GLint shaderProgram;
GLint skyboxShader;


//========================[ Objects ]==========================//
Cube * skyBox;
OBJObject * bear;
OBJObject * island;
//===============[ Default camera parameters ]================//
glm::vec3 eye_pos;
glm::vec3 look_dir;
glm::mat4 matrix;

glm::vec3 cam_pos(0.0f, 100.0f, 350.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, -100.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

bool clicked = false;
glm::vec3 lastPoint;                        // coordinates of the last position
glm::vec3 curPoint;                         // coordinates of the current position
GLfloat fov =  45.0f;
//========================[ Colors ]=========================//
//material color: gold
glm::vec3 matAmb(0.329412,0.223529,0.027451);
glm::vec3 matDiff(0.780392, 0.568627, 0.113725);
glm::vec3 matSpec(0.992157, 0.941176, 0.807843);
float matShine = 3;

// directional & point light
glm::vec3 dirAmb(0.8f, 0.8f, 0.8f);
glm::vec3 dirDiff(0.50f, 0.50f, 0.50f);
glm::vec3 dirSpec(0.8f, 0.8f, 0.8f);
glm::vec3 dirLight_dir(0, -30, -30);
//===========================================================//
void Window::initialize_objects()
{
    skyBox = new Cube();
    //skyBox->scale(40);
    bear = new OBJObject(("bear.obj"));
    island = new OBJObject("Island/dream_island.obj");
    island->translate(0.0f, -100.0f, 0.0f);
    
    shaderProgram = LoadShaders("shader.vert", "shader.frag");
    skyboxShader = LoadShaders("skyShader.vert", "skyShader.frag");
}

void Window::clean_up()
{
    delete(bear);
    delete(skyBox);
    delete(island);

	glDeleteProgram(shaderProgram);
    glDeleteProgram(skyboxShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0) {
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
    //=======================[ Controls camera ]================================//
    if ( clicked && (lastPoint.x != curPoint.x || lastPoint.y != curPoint.y)) {
        
        glm::vec3 curPoint_3d = trackBallMapping( curPoint.x, curPoint.y );    // 2d position -> sphere location.
        glm::vec3 lastPoint_3d = trackBallMapping( lastPoint.x, lastPoint.y );
        
        glm::vec3 direction = curPoint_3d - lastPoint_3d;
        float velocity = glm::length(direction);
        
        if( velocity > 0.001 ) {      // If little movement - do nothing.
            glm::vec3 rotAxis = glm::cross( lastPoint_3d, curPoint_3d );

            float rot_angle = glm::acos( glm::min(1.0f, glm::dot(curPoint_3d, lastPoint_3d)) ) * 0.01;
            skyBox->angle = rot_angle;

            glm::vec4 new_cam = glm::vec4(cam_pos, 1.0f) ;
            new_cam = new_cam * glm::rotate( glm::degrees(skyBox->angle), rotAxis );

            cam_pos.x = new_cam.x;
            cam_pos.y = new_cam.y;
            cam_pos.z = new_cam.z;
            
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        }
    }
    lastPoint = curPoint;

}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(skyboxShader);
    skyBox->draw(skyboxShader);
    
	glUseProgram(shaderProgram);

    glm::vec3 curPoint; // direction of light follow by mouse

    //============================ Material ============================//
    GLuint materialID = glGetUniformLocation(shaderProgram, "material.ambient");
    glUniform3fv(materialID, 1, (GLfloat*) &matAmb);
    materialID = glGetUniformLocation(shaderProgram, "material.diffuse");
    glUniform3fv(materialID, 1, (GLfloat*) &matDiff);
    materialID = glGetUniformLocation(shaderProgram, "material.specular");
    glUniform3fv(materialID, 1, (GLfloat*) &matSpec);
    materialID = glGetUniformLocation(shaderProgram, "material.shininess");
    glUniform1f(materialID, matShine);
    //========================== Direct light ==========================//
    materialID = glGetUniformLocation(shaderProgram, "dirLight.ambient");
    glUniform3fv(materialID, 1, (GLfloat*) &dirAmb);
    materialID = glGetUniformLocation(shaderProgram, "dirLight.diffuse");
    glUniform3fv(materialID, 1, (GLfloat*) &dirDiff);
    materialID = glGetUniformLocation(shaderProgram, "dirLight.specular");
    glUniform3fv(materialID, 1, (GLfloat*) &dirSpec);
    materialID = glGetUniformLocation(shaderProgram, "dirLight.direction");
    glUniform3fv(materialID, 1, (GLfloat*) &dirLight_dir );
    
    island->draw(shaderProgram);
    
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ( action == GLFW_PRESS || action == GLFW_REPEAT ) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
        clicked = true;
    else
        clicked = false;
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    lastPoint.x = curPoint.x;
    lastPoint.y = curPoint.y;
    lastPoint.z = 0;
    
    curPoint.x = xpos;
    curPoint.y = ypos;
    curPoint.z = 0;
}

// Zoom in and out of the screen.
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    glm::vec3 dir = glm::normalize( glm::vec3 (0.0f) - cam_pos ) ;
    
    if (yoffset < 0)
        dir = -dir;
    
    glm::mat4 trans = glm::mat4 (1.0f);
    trans[3][0] = dir.x;
    trans[3][1] = dir.y;
    trans[3][2] = dir.z;

    glm::vec4 new_cam = trans * glm::vec4(cam_pos, 1.0f) ;
    cam_pos.x = new_cam.x;
    cam_pos.y = new_cam.y;
    cam_pos.z = new_cam.z;

    V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

// Utility routine to calculate the 3D position of a projected unit vector onto the xy-plane.
glm::vec3 Window::trackBallMapping(double x, double y)
{
    glm::vec3 coord(0.0f);
    
    coord.x =  (2 * x - width ) / width;
    coord.y = -(2 * y - height) / height;
    
    /* Clamp it to border of the windows, comment these codes to allow rotation when cursor is not over window */
    coord.x = glm::clamp( coord.x, -1.0f, 1.0f );
    coord.y = glm::clamp( coord.y, -1.0f, 1.0f );
    
    float length_squared = coord.x * coord.x + coord.y * coord.y;
    
    if( length_squared <= 1.0 )
        coord.z = sqrt( 1.0 - length_squared );
    else
        coord = glm::normalize( coord );
    
    return coord;
}

// m_pos = Translate vector, m_scale = resize vectore, m_angle = angle to rotate, axis type 1=x, 2=y, 3=z
glm::mat4 Window::getMatrix(glm::vec3 m_pos, glm::vec3 m_scale, float m_angle, int axis)
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_pos);
    glm::mat4 scale = glm::scale(glm::mat4(1.0), m_scale);
    glm::mat4 rotate = glm::mat4(1.0f);
    
    if (axis == 1)
        rotate = glm::rotate(glm::mat4(1.0f), -m_angle/ 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    else if (axis == 2)
        rotate = glm::rotate(glm::mat4(1.0f), -m_angle/ 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    else if (axis == 3)
        rotate = glm::rotate(glm::mat4(1.0f), -m_angle/ 180.0f * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));

    return translate * scale * rotate;
}



