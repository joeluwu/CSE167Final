//
//  Node.h
//  PA3
//
//  Created by Joe Lu Wu on 5/4/16.
//  Copyright © 2016 Joe Lu Wu. All rights reserved.
//

#ifndef Node_h
#define Node_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <stdio.h>
#include <list>
#include "OBJObject.h"

class Node
{
public:
    virtual void update(glm::mat4 C) = 0;
    virtual void draw(GLuint shader) = 0;
};

#endif /* Node_h */





