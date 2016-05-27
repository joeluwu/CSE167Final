#include "window.h"

const char* window_title = "GLFW Starter Project";
Cube * cube;
OBJObject * bunny;
OBJObject * bear;
OBJObject * dragon;
OBJObject * obj_ptr;
GLint shaderProgram;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
glm::mat4 Window::P;
glm::mat4 Window::V;

int Window::width;
int Window::height;
bool isRotate;                              // if the model needs to rotate
bool isTrans;                               // if the mdoel needs to translate
int lightType = 0;                          // 0.no light, 1. direction,   2. point,   3. spot

glm::vec3 lastPoint;                        // coordinates of the last position
glm::vec3 curPoint;                         // coordinates of the current position

///////////////////////=============colors=============///////////////////////
//material color: gold
glm::vec3 matAmb(0.24725,0.1995,0.0745);
glm::vec3 matDiff(0.7516, 0.60648, 0.22648);
glm::vec3 matSpec(0.628281, 0.555802, 0.366065);
float matShine = 40;

// directional & point light
glm::vec3 dirAmb(0.04f, 0.04f, 0.5f);
glm::vec3 dirDiff(0.50f, 0.50f, 0.50f);
glm::vec3 dirSpec(0.8f, 0.8f, 0.8f);
glm::vec3 dirLight_dir(0, 0, -1);

glm::vec3 point_dir(0, 0, 50);
glm::vec3 new_dir(0, 0, 50);

glm::vec3 dirInt(0.1f, 0.1f, 0.1f);
glm::vec3 pos(0, 0, 20);
float quadratic = 40;

// spot light
glm::vec3 position;
glm::vec3 direction;
float spot_cutOff = glm::radians(10.0f);
float spot_exp = 0;

//////////////////////////////////////////////////////////////////////////////

void Window::initialize_objects()
{
    bunny = new OBJObject(("/Users/Joe/Dropbox/2016 Spring/CSE 167/PA2/PA2/bunny.obj"));
    bear = new OBJObject(("/Users/Joe/Dropbox/2016 Spring/CSE 167/PA2/PA2/bear.obj"));
    dragon = new OBJObject(("/Users/Joe/Dropbox/2016 Spring/CSE 167/PA2/PA2/dragon.obj"));

    obj_ptr = bunny;

	// Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	shaderProgram = LoadShaders("/Users/Joe/Dropbox/2016 Spring/CSE 167/PA2/shader.vert",
                                "/Users/Joe/Dropbox/2016 Spring/CSE 167/PA2/shader.frag");
#else // Not windows
	shaderProgram = LoadShaders("shader.vert", "shader.frag");
#endif
}

void Window::clean_up()
{
    delete(bunny);
    delete(bear);
    delete(dragon);
    
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
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
	if (!window)
	{
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

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
    if ( isRotate && (lastPoint.x != curPoint.x || lastPoint.y != curPoint.y) ) {
        
        if (lightType == 0){
            glm::vec3 curPoint_3d = trackBallMapping( curPoint.x, curPoint.y );    // Map 2d position to a sphere location.
            glm::vec3 lastPoint_3d = trackBallMapping( lastPoint.x, lastPoint.y ); // Map 2d position to a sphere location.
        
            glm::vec3 direction = curPoint_3d - lastPoint_3d;
        
            float velocity = glm::length(direction);
        
            if( velocity > 0.001 ) // If little movement - do nothing.
            {
                // Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
                glm::vec3 rotAxis = glm::cross( lastPoint_3d, curPoint_3d );
                float rot_angle = glm::acos( glm::min(1.0f, glm::dot(curPoint_3d, lastPoint_3d)) ) * 0.3;
            
                obj_ptr->angle = rot_angle;
                if (obj_ptr->angle > 360.0f || obj_ptr->angle < -360.0f)
                    obj_ptr->angle = 0.0f;
            
                obj_ptr->toWorld = glm::rotate( glm::mat4(1.0f), glm::degrees(rot_angle), rotAxis ) * obj_ptr->toWorld;
            }
            
        } else if (lightType == 1){

            dirLight_dir = trackBallMapping(lastPoint.x, lastPoint.y);
            dirLight_dir.x *= -1;
            dirLight_dir.y *= -1;
            dirLight_dir.z *= -1;
            
        }else if (lightType == 2 || lightType == 3) {
            // point light
            // Rotations rotate the light around the origin of the world coordinate system (=center of window).
            // Scaling with the mouse wheel changes the distance of the light from the origin of the world coordinate system.
            // Light intensity should be visibly different on the model as the light is moved closer & farther from the model
            // surface.
            
            glm::vec3 curPoint_3d = trackBallMapping( curPoint.x, curPoint.y );    // Map 2d position to a sphere location.
            glm::vec3 lastPoint_3d = trackBallMapping( lastPoint.x, lastPoint.y ); // Map 2d position to a sphere location.
            
            glm::vec3 direction = curPoint_3d - lastPoint_3d;
            
            float velocity = glm::length(direction);
            
            if( velocity > 0.001 ) // If little movement - do nothing.
            {
                // Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
                glm::vec3 rotAxis = glm::cross( lastPoint_3d, curPoint_3d );
                float rot_angle = glm::acos( glm::min(1.0f, glm::dot(curPoint_3d, lastPoint_3d)) ) * 0.3;
                
                glm::vec4 result = glm::rotate( glm::mat4(1.0f), glm::degrees(rot_angle), rotAxis ) * glm::vec4(point_dir, 1);
                point_dir.x = result.x;
                point_dir.y = result.y;
                point_dir.z = result.z;
            }
            new_dir = glm::vec3(point_dir.x*20, point_dir.y*20, point_dir.z * 20);
        }
        
    } else if (isTrans) {
        
        if(lightType == 3) {   //make the spot light wider/narrower
            
            if ( (curPoint.y - lastPoint.y) < 0 )
                //spot_cutOff += glm::radians(10.0f);
                spot_cutOff += 0.002f;
            else
                //spot_cutOff -= glm::radians(10.0f);
                spot_cutOff -= 0.002f;

        } else if(lightType == 0){
            float offsetX = (curPoint.x - lastPoint.x) / 40;
            float offsetY = (curPoint.y - lastPoint.y) / 40;
            obj_ptr->translate(offsetX, -offsetY, 0);
        }
    }
    lastPoint = curPoint;
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
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
    
    //glm::vec3 curPoint = trackBallMapping(lastPoint.x, lastPoint.y);
    materialID = glGetUniformLocation(shaderProgram, "dirLight.direction");
    glUniform3fv(materialID, 1, (GLfloat*) &dirLight_dir );
    
    materialID = glGetUniformLocation(shaderProgram, "dirLight.on");
    glUniform1i(materialID, false);
    
    //========================== Point light ==========================//
    materialID = glGetUniformLocation(shaderProgram, "pointLight.ambient");
    glUniform3fv(materialID, 1, (GLfloat*) &dirAmb);
    
    materialID = glGetUniformLocation(shaderProgram, "pointLight.diffuse");
    glUniform3fv(materialID, 1, (GLfloat*) &dirDiff);
    
    materialID = glGetUniformLocation(shaderProgram, "pointLight.specular");
    glUniform3fv(materialID, 1, (GLfloat*) &dirSpec);
    
    materialID = glGetUniformLocation(shaderProgram, "pointLight.position");
    glUniform3fv(materialID, 1, (GLfloat*) &point_dir );
    
    materialID = glGetUniformLocation(shaderProgram, "pointLight.on");
    glUniform1i(materialID, false);
    
    //========================== Spot light ==========================//
    materialID = glGetUniformLocation(shaderProgram, "spotLight.diffuse");
    glUniform3fv(materialID, 1, (GLfloat*) &dirDiff);
    
    materialID = glGetUniformLocation(shaderProgram, "spotLight.specular");
    glUniform3fv(materialID, 1, (GLfloat*) &dirSpec);
    
    materialID = glGetUniformLocation(shaderProgram, "spotLight.ambient");
    glUniform3fv(materialID, 1, (GLfloat*) &dirAmb);
    
    //glm::vec3 curPoint = trackBallMapping(lastPoint.x, lastPoint.y);
    materialID = glGetUniformLocation(shaderProgram, "spotLight.direction");
    glUniform3fv(materialID, 1, (GLfloat*) &point_dir);
    
    materialID = glGetUniformLocation(shaderProgram, "spotLight.position");
    glUniform3fv(materialID, 1, (GLfloat*) &point_dir);
    
    materialID = glGetUniformLocation(shaderProgram, "spotLight.cutOff");
    glUniform1f(materialID, spot_cutOff);
    
    materialID = glGetUniformLocation(shaderProgram, "spotLight.exponent");
    glUniform1f(materialID, spot_exp);
    
    materialID = glGetUniformLocation(shaderProgram, "spotLight.on");
    glUniform1i(materialID, false);
    
    //=================== detect which light is on ===================//
    
    if( lightType == 1){           // directional light
        materialID = glGetUniformLocation(shaderProgram, "dirLight.on");
        glUniform1i(materialID, true);
    }
    if (lightType == 2) {   // point light
        materialID = glGetUniformLocation(shaderProgram, "pointLight.on");
        glUniform1i(materialID, true);
    }
    if (lightType == 3) {   // spot light
        materialID = glGetUniformLocation(shaderProgram, "spotLight.on");
        glUniform1i(materialID, true);
    }
    
    obj_ptr->draw(shaderProgram);
    
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if ( action == GLFW_PRESS || action == GLFW_REPEAT )
	{
        switch (key) {
            case GLFW_KEY_ESCAPE: // Close the window. Terminate program.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_F1:
                obj_ptr = bear;
                break;
            case GLFW_KEY_F2:
                obj_ptr = bunny;
                break;
            case GLFW_KEY_F3:
                obj_ptr = dragon;
                break;
            case GLFW_KEY_0:        // de-select and return to controlling the 3D model
                lightType = 0;
                break;
            case GLFW_KEY_1:        // Direct light
                lightType = 1;
                // TODO ;
                break;
            case GLFW_KEY_2:        // Point light
                lightType = 2;
                break;
            case GLFW_KEY_3:        // Spot light
                lightType = 3;
                break;
            case GLFW_KEY_E:        // change the spot light edge to be sharper/blurrier.
                // TODO  change the exponent, which would then change the attenuation
                if (mods == GLFW_MOD_SHIFT) {
                    spot_exp += 0.02f;
                } else {
                    //if (spot_exp >= 0.02f)
                        spot_exp -= 0.01f;
                }
                break;
            case GLFW_KEY_X:        // move left/right
                if (mods == GLFW_MOD_SHIFT)
                    obj_ptr->translate(1.0, 0, 0);
                else
                    obj_ptr->translate(-1.0, 0, 0);
                break;
            case GLFW_KEY_Y:        // move down/up
                if (mods != GLFW_MOD_SHIFT)
                    obj_ptr->translate(0, 1.0, 0);
                else
                    obj_ptr->translate(0, -1.0, 0);
                break;
            case GLFW_KEY_Z:        // move into/out of the screen
                if (mods == GLFW_MOD_SHIFT)
                    obj_ptr->translate(0, 0, 1.0);
                else
                    obj_ptr->translate(0, 0, -1.0);
                break;
            case GLFW_KEY_S:        // scale down/up about the model's center
                if (mods == GLFW_MOD_SHIFT)
                    obj_ptr->scale(1.1);
                else
                    obj_ptr->scale(0.96);
                break;
            case GLFW_KEY_P:        // change point size
                if (mods == GLFW_MOD_SHIFT)
                    obj_ptr->p_size(1);
                else
                    obj_ptr->p_size(-1);
                break;
            case GLFW_KEY_R: // reset position, orientation and size
                obj_ptr->reset();
                break;
            default:
                break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////
void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT){
        isRotate = true;
        isTrans = false;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT) {
        isRotate = false;
        isTrans = true;
    } else {
        isRotate = false;
        isTrans = false;
    }
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    lastPoint.x = curPoint.x;
    lastPoint.y = curPoint.y;
    lastPoint.z = 0;
    
    curPoint.x = xpos;
    curPoint.y = ypos;
    lastPoint.z = 0;
}

// move the model along the screen space z axis (i.e., in and out of the screen).
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (lightType == 2 || lightType == 3){
        
        glm::vec3 flip = new_dir;
        
        if (yoffset < 0)
            flip = -new_dir;
                    
        flip = glm::normalize(flip);
        
        glm::mat4 trans = glm::mat4(1.0f);
        trans[3][0] = flip.x;
        trans[3][1] = flip.y;
        trans[3][2] = flip.z;

        glm::vec4 result = trans * glm::vec4(point_dir, 1.0);
        point_dir.x = result.x;
        point_dir.y = result.y;
        point_dir.z = result.z;
        
    }else if (lightType == 0){
        obj_ptr->translate(0, 0, yoffset/(-3));
    }
}

// Utility routine to calculate the 3D position of a projected unit vector onto
// the xy-plane. Given any point on the xy-plane, we can think of it as the projection
// from a sphere down onto the plane. The inverse is what we are after.
glm::vec3 Window::trackBallMapping(double x, double y)
{
    glm::vec3 v;
    float d;
    v.x = (2.0 * x - Window::width) / Window::width;
    v.y = (Window::height - 2.0 * y) / Window::height;
    v.z = 0.0;
    d = glm::length(v);
    d = (d < 1.0) ? d : 1.0;
    v.z = sqrtf(1.001 - d*d);
    return glm::normalize(v); //  need to normalize, since we only capped d, not v.
}
