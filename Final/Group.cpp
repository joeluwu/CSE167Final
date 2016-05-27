//
//  Group.cpp
//  PA3
//
//  Created by Joe Lu Wu on 5/4/16.
//  Copyright © 2016 Joe Lu Wu. All rights reserved.
//

#include <stdio.h>
#include <list>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Node.h"

using namespace std;

class Group: public Node {
    
public:
    list<Node*> children;

    virtual void update(glm::mat4 C) {
        
        for( list<Node *>::iterator it = children.begin(); it != children.end(); ++it )
            (*it)->update(C);
    }
    
    virtual void draw(GLuint shader) {

        for( list<Node *>::iterator it = children.begin(); it != children.end(); ++it )
            (*it)->draw(shader);
    }
    
    void addChild(Node* n){
        children.push_back( n );
    }
    void removeChild(Node* n){
        // TODO
        children.remove(n);
    }
};