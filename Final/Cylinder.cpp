//
//  Cylinder.cpp
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
#include "Geode.h"

class Cylinder: public Geode {
    
public:
    
    Cylinder(OBJObject *cylinder) {
        toDraw = cylinder;
    }
};