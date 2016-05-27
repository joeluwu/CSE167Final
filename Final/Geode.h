//
//  Geode.h
//  PA3
//
//  Created by Joe Lu Wu on 5/4/16.
//  Copyright © 2016 Joe Lu Wu. All rights reserved.
//

#ifndef Geode_h
#define Geode_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <stdio.h>
#include "OBJObject.h"
#include "Node.h"

class Geode: public Node {
    
public:
    OBJObject *toDraw;
    glm::mat4 M;
    
    void update(glm::mat4 C);
    void draw(GLuint shader);
};




#endif /* Geode_h */
