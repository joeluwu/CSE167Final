//
//  WavefrontSection.hpp
//  Final
//
//  Created by Joe Lu Wu on 6/3/16.
//  Copyright © 2016 Joe Lu Wu. All rights reserved.
//

#ifndef WavefrontSection_hpp
#define WavefrontSection_hpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <stdio.h>
#include <String>
#include <Vector>

using namespace std;

class WavefrontSection
{
public:
    WavefrontSection(string mat_name = ""){ material = NULL; }
    void ConstructVAO();
    void Render();
    
    std::vector<float> vert;
    std::vector<float> tex_coord;
    std::vector<float> norm;
    
    /* these are the indexes used to build the VAO */
    std::vector<int>   vert_index;
    std::vector<int>   norm_index;
    std::vector<int>   tex_index;
    
    /* This is the material, containing ambient, diffuse and specular materials,
     and texture */
    Material *material;
    
    /* This is the VAO */
    VAO vao;
};
#endif /* WavefrontSection_hpp */
