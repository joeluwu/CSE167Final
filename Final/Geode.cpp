//
//  Geode.cpp
//  PA3
//
//  Created by Joe Lu Wu on 5/4/16.
//  Copyright © 2016 Joe Lu Wu. All rights reserved.
//

#include <stdio.h>
#include "Geode.h"

void Geode::update(glm::mat4 C) {
    
    M = C;
}

void Geode::draw(GLuint shader) {
    
    toDraw->draw(shader, M);
}
