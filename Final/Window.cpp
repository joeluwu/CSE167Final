#include "window.h"

const char* window_title = "GLFW Starter Project";
glm::mat4 Window::P;
glm::mat4 Window::V;
int Window::width;
int Window::height;

const float ROTATE_SPEED = 3;    // change the speed of the rotate motion
const float UP_SPEED = 0.1;      // change the speed of the up down motion

GLint shaderProgram;
GLint skyboxShader;

float angle = 0;                 // rotation angle for the whole world
bool isGoUp;                     // decide direction of going up or down
float up = -2.5;                 // lower bound of up-down movement
bool isGoOut;                    // decide direction of extending out
float out = 0;                   // lower bound of extention movement
//========================[ Objects ]==========================//
Cube * skyBox;
OBJObject * bear;
OBJObject * cylinder;
OBJObject * pod;

MatrixTransform *world;
MatrixTransform *mt_bear;
MatrixTransform *center;
MatrixTransform *base;
MatrixTransform *level1;
MatrixTransform *level2;
MatrixTransform *level3;
MatrixTransform* mt_top;                  // top center pole for contraction

vector<MatrixTransform*> pods;            // store all the pods so can make them spin
vector<MatrixTransform*> flowers;         // store all (3 pods and 3 arms)
vector<MatrixTransform*> pod_arms;        // store all pod & arms, extend them
vector<MatrixTransform*> arms;            // store all 9 big arms, extend them

//===============[ Default camera parameters ]================//
int camType = 0;
glm::vec3 eye_pos;
glm::vec3 look_dir;
glm::mat4 matrix;

glm::vec3 cam_pos(0.0f, 20.0f, 70.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 20.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

bool clicked = false;
glm::vec3 lastPoint;                        // coordinates of the last position
glm::vec3 curPoint;                         // coordinates of the current position
GLfloat fov =  45.0f;
//========================[ Colors ]=========================//
//material color: gold
glm::vec3 matAmb(0.24725,0.1995,0.0745);
glm::vec3 matDiff(0.7516, 0.60648, 0.22648);
glm::vec3 matSpec(0.628281, 0.555802, 0.366065);
float matShine = 40;

// directional & point light
glm::vec3 dirAmb(0.8f, 0.8f, 0.8f);
glm::vec3 dirDiff(0.50f, 0.50f, 0.50f);
glm::vec3 dirSpec(0.8f, 0.8f, 0.8f);
glm::vec3 dirLight_dir(0, -30, -30);
//===========================================================//
void Window::initialize_objects()
{
    skyBox = new Cube();
    skyBox->scale(500);
    pod = new OBJObject(("/Users/Joe/Dropbox/2016 Spring/CSE 167/PA3/PA3/pod.obj"));
    bear = new OBJObject(("/Users/Joe/Dropbox/2016 Spring/CSE 167/PA3/PA3/bear.obj"));
    cylinder = new OBJObject(("/Users/Joe/Dropbox/2016 Spring/CSE 167/PA3/PA3/cylinder.obj"));
    shaderProgram = LoadShaders("shader.vert", "shader.frag");
    skyboxShader = LoadShaders("skyShader.vert", "skyShader.frag");

    //===============================[ Base ]===============================//
    base = new MatrixTransform( );
    Cylinder* c = new Cylinder( cylinder );
    Cylinder* small_bear = new Cylinder( bear );
    mt_bear = new MatrixTransform( getMatrix(glm::vec3(0.1, -0.5, -0.3), glm::vec3(0.07f, 0.07f, 0.07f), 0, 0));
    mt_bear->addChild(small_bear);
    
    base->addChild( c );
    
    world = new MatrixTransform();

    center = build_center();
    level1 = build_level( 0, 3 );
    level2 = build_level( 60, 15);
    level3 = build_level( 0, 30 );
    
    world->addChild(center);
    world->addChild(level1);
    world->addChild(level2);
    world->addChild(level3);
    
    pods.back()->addChild(mt_bear);
}

void Window::clean_up()
{
    delete(bear);
    delete(skyBox);
    delete(cylinder);
    delete(pod);

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

            float rot_angle = glm::acos( glm::min(1.0f, glm::dot(curPoint_3d, lastPoint_3d)) ) * 0.03;
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
    
    angle += ROTATE_SPEED;
    if (angle >= 360) angle = 0;
    
    //==========================[ spin the pod ]==========================//
    
    for(vector<MatrixTransform*>::iterator itr = pods.begin(); itr != pods.end(); ++itr)
        (*itr)->M = getMatrix(glm::vec3(5.0f, 0.5, 0), glm::vec3(1.0f), angle, 2);

    matrix = mt_bear->M;
    
    glm::mat4 mt_fixed = getMatrix(glm::vec3(3.6, 0, 0), glm::vec3(3.5, 0.25, 0.25), 90, 3);
    matrix *= mt_fixed;
    
    glm::vec3 eye = glm::vec3 (0, 3, 0);
    glm::vec3 look = glm::vec3 (0, 1, 1);
    eye_pos = glm::vec3( matrix * glm::vec4( eye, 1.0f) );
    look_dir = glm::vec3( matrix * glm::vec4( look, 1.0f) );

    //=======================[ spin the flowers ]=======================//
    
    for(vector<MatrixTransform*>::iterator itr = flowers.begin(); itr != flowers.end(); ++itr)
        (*itr)->M = getMatrix(glm::vec3(25.0f, 0.0f, 0.0f), glm::vec3(1.0f), angle, 2);
    
    //=======================[ extend small arms ]=======================//
    if(out >= 5 || out <= -2.5) isGoOut = !isGoOut;
    if(isGoOut) out += UP_SPEED;
    else       out -= UP_SPEED;
    for(vector<MatrixTransform*>::iterator itr = pod_arms.begin(); itr != pod_arms.end(); ++itr)
        (*itr)->M = getMatrix(glm::vec3( (8+out)/1.3, 0, 0), glm::vec3(1.0f), 0, 1);
    
    //========================[ extend big arms ]========================//
    
    if(up >= 5 || up <= -2.5) isGoUp = !isGoUp;
    if(isGoUp) up += UP_SPEED;
    else       up -= UP_SPEED;
    
    for(vector<MatrixTransform*>::iterator itr = arms.begin(); itr != arms.end(); ++itr)
        (*itr)->M = getMatrix( glm::vec3(-4.5+up, 0, 0), glm::vec3(1.0f), 0, 3);
    
    //========================[ levels up-down ]========================//

    level1->M = getMatrix(glm::vec3(0.0f, up, 0.0f), glm::vec3(1.0f), 0, 0) ;
    level2->M = getMatrix(glm::vec3(0.0f, 18 - up, 0.0f), glm::vec3(1.0f), 60, 2) ;
    level3->M = getMatrix(glm::vec3(0.0f, 32+up, 0.0f), glm::vec3(1.0f), 0, 0) ;
    
     mt_top->M= getMatrix(glm::vec3(0, 23+up, 0), glm::vec3(0.7f, 7.0f, 0.7f), 0, 1);

    //=====================[ Rotate the whole world ]======================//
    
    world->update( getMatrix(glm::vec3(0.0f), glm::vec3(1.0f), angle, 2) );
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
    
    world->draw(shaderProgram);
    
    base->update(getMatrix(glm::vec3(0, -6, 0), glm::vec3(70.0f, 0.2f, 50.0f), 0, 1));
    base->draw(shaderProgram);
    
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
        if (key == GLFW_KEY_C) {
            if (camType == 0) {
                V = glm::lookAt(cam_pos, cam_look_at, cam_up);
                camType = 1;
            } else {
                
                V = glm::lookAt(eye_pos, look_dir, cam_up);
                camType = 0;
            }
        }
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

MatrixTransform* Window::build_pod(float rot_angle){
    
    MatrixTransform* mt_whole = new MatrixTransform( getMatrix(glm::vec3(0.0f), glm::vec3(1.0f), rot_angle, 2) );

    MatrixTransform* mt_moving = new MatrixTransform( getMatrix(glm::vec3(0.0f), glm::vec3(1.0f), rot_angle, 2) );

    // non-movable part of arm
    MatrixTransform* mt_fixed = new MatrixTransform( getMatrix(glm::vec3(3.6, 0, 0), glm::vec3(3.5, 0.25, 0.25), 90, 3) );
    Cylinder* fixed = new Cylinder( cylinder );
    mt_fixed->addChild( fixed );
    
    // movable part of arm
    MatrixTransform* mt_c = new MatrixTransform( getMatrix(glm::vec3(0.52, 0, 0), glm::vec3(4, 0.25, 0.25), 90, 3) );
    Cylinder* c = new Cylinder( cylinder );
    mt_c->addChild( c );
    
    // pod
    MatrixTransform* mt_p = new MatrixTransform( getMatrix(glm::vec3(5.0f, 0.5, 0), glm::vec3(1.0f), 0, 0) );
    Pod* p = new Pod( pod );
    mt_p->addChild( p );
    pods.push_back(mt_p);
    
    // attach arm and pod
    mt_moving->addChild(mt_c);
    mt_moving->addChild(mt_p);
    
    pod_arms.push_back(mt_moving);

    mt_whole->addChild(mt_moving);
    mt_whole->addChild(mt_fixed);

    return mt_whole;
}

MatrixTransform* Window::build_flower(){
    
    MatrixTransform *mt_flower = new MatrixTransform( getMatrix(glm::vec3(25.0f, 0.0f, 0.0f), glm::vec3(1.0f), 0, 2) );
    
    // round = 3 of (Pod & cylinder) combo
    MatrixTransform* mt_pod1 = build_pod( 0 );
    MatrixTransform* mt_pod2 = build_pod( 120 );
    MatrixTransform* mt_pod3 = build_pod( 240 );
    
    mt_flower->addChild( mt_pod1 );
    mt_flower->addChild( mt_pod2 );
    mt_flower->addChild( mt_pod3 );
    
    flowers.push_back(mt_flower);
    
    return mt_flower;
}

MatrixTransform* Window::build_arm( float rot_angle ){
    
    // whole = big arm + moving part, moving part = small arm + flower
    MatrixTransform* mt_whole = new MatrixTransform(getMatrix(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), rot_angle, 2));
    
    // movie part = small arm + flower
    MatrixTransform* mt_moving = new MatrixTransform();
    
    MatrixTransform* mt_small = new MatrixTransform(getMatrix( glm::vec3(21, -0.7, 0), glm::vec3(4.0f, 0.5f, 0.5f), 90, 3) );
    Cylinder* small_arm = new Cylinder( cylinder );
    mt_small->addChild( small_arm );
    mt_moving->addChild( mt_small );
    
    MatrixTransform *mt_flr = build_flower();
    mt_moving->addChild(mt_flr);

    arms.push_back(mt_moving);
    
    // Build and attach large arm
    MatrixTransform* mt_arm = new MatrixTransform( getMatrix( glm::vec3(9, -0.7, 0), glm::vec3(9.0f, 0.5f, 0.5f), 90, 3) );
    Cylinder* arm = new Cylinder( cylinder );
    mt_arm->addChild(arm);

    mt_whole->addChild(mt_moving);
    mt_whole->addChild(mt_arm);

    return mt_whole;
}

MatrixTransform* Window::build_level( float rot_angle, float height_offset ){

    MatrixTransform *mt_whole = new MatrixTransform(getMatrix(glm::vec3(0.0f, height_offset, 0.0f), glm::vec3(1.0f), rot_angle, 2));
    MatrixTransform *mt_arm1 = build_arm(0);
    MatrixTransform *mt_arm2 = build_arm(120);
    MatrixTransform *mt_arm3 = build_arm(240);
    
    mt_whole->addChild(mt_arm1);
    mt_whole->addChild(mt_arm2);
    mt_whole->addChild(mt_arm3);

    return mt_whole;
}

MatrixTransform* Window::build_center(){
    
    MatrixTransform* mt_whole = new MatrixTransform( getMatrix(glm::vec3(0, 1, 0), glm::vec3(1.0f), 0, 1) );
    
    MatrixTransform* mt_bottom = new MatrixTransform( getMatrix(glm::vec3(0, 7, 0), glm::vec3(0.7f, 14.0f, 0.7f), 0, 1)  );
    mt_top = new MatrixTransform( getMatrix(glm::vec3(0, 28, 0), glm::vec3(0.8f, 16.0f, 0.8f), 0, 1) );

    Cylinder* c_bottom = new Cylinder( cylinder );
    Cylinder* c_top = new Cylinder( cylinder );

    mt_bottom->addChild( c_bottom );
    mt_top->addChild( c_top );

    mt_whole->addChild(mt_bottom);
    mt_whole->addChild(mt_top);
 
    return mt_whole;
}