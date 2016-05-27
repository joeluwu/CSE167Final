//
//  MatrixTransform.cpp
//  PA3
//
//  Created by Joe Lu Wu on 5/4/16.
//  Copyright © 2016 Joe Lu Wu. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <stdio.h>
#include <iostream>
#include "Group.cpp"

using namespace std;

class MatrixTransform: public Group {

public:
    glm::mat4 M;
    
    // constructor
    MatrixTransform (){
        M = glm::mat4(1.0f);
    }
    // copy constructor
    MatrixTransform (glm::mat4 C){
        M = C;
    }
    
    void update(glm::mat4 C){
//
//        M = C * M;
//        Group::update(M);
        glm::mat4 temp = C * M;
        Group::update( temp );
    }
    void draw(GLuint shader) {
        
        for( list<Node *>::iterator it = children.begin(); it != children.end(); ++it )
            (*it)->draw(shader);
    }
};
