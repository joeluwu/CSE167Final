#include "OBJObject.h"
#include "Window.h"

#include <iostream>
#include <cfloat>
using namespace std;

OBJObject::OBJObject(const char *filepath) 
{
    toWorld = glm::mat4(1.0f);
    this->angle = 0.0f;
    maxX = FLT_MIN;
    maxY = FLT_MIN;
    maxZ = FLT_MIN;
    minX = FLT_MAX;
    minY = FLT_MAX;
    minZ = FLT_MAX;
    parse(filepath);
    
    float avgX = (maxX + minX) / 2;
    float avgY = (maxY + minY) / 2;
    float avgZ = (maxZ + minZ) / 2;
    
    float diffX = maxX - minX;
    float diffY = maxY - minY;
    float diffZ = maxZ - minZ;
    float largest = 1;
    
    if( diffX >= diffY && diffX >= diffZ ){
        largest = diffX;
    } else if( diffY >= diffX && diffY >= diffZ ){
        largest = diffY;
    } else if( diffZ >= diffX && diffZ >= diffY ){
        largest = diffZ;
    }
    
    // recenter all the vertices
    for (unsigned int i = 0; i < vertices.size(); ++i) {
        vertices[i].x -= avgX;
        vertices[i].x /= largest;
        vertices[i].y -= avgY;
        vertices[i].y /= largest;
        vertices[i].z -= avgZ;
        vertices[i].z /= largest;
    }

    ///////////////////////////////////////////////////////
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);     // Vertex Array Object, ties together a multitude of buffers
    glGenBuffers(1, &VBO);          // Vertex Buffer Object (vertices)
    glGenBuffers(1, &EBO);          // Element Buffer Object (face indices)
    glGenBuffers(1, &NBO);          // Element Buffer Object (face indices)
    
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()* sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}


void OBJObject::parse(const char *filepath) 
{
    // parse the OBJ file, Populate the face indices, vertices, 
    // normals vectors and faces with the OBJ Object data
    float x,y,z;     // vertex coordinates
    float r,g,b;     // vertex color
    glm::vec3 vertex, color, norm;
    
    FILE* fp = fopen(filepath,"rb");

    if (fp == NULL) {
        cerr << "error loading file" << endl;
        exit(-1); 
    }  

    while (1) {
        char type[128]; //the first few characters on each line

        int res = fscanf(fp, "%s", type);
        if (res == EOF)    break; 
 
        if ( strcmp( type, "v" ) == 0){

            fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);

            vertex.x = x;
            vertex.y = y;
            vertex.z = z;

            color.x = x;
            color.y = y;
            color.z = z;
            
            if( x > maxX )
                maxX = x;
            else if( x < minX )
                minX = x;
            if( y > maxY )
                maxY = y;
            else if( y < minY )
                minY = y;
            if( z > maxZ )
                maxZ = z;
            else if( z < minZ )
                minZ = z;
            
            vertices.push_back(vertex);
            colors.push_back(color);
            
        } else if ( strcmp( type, "vn" ) == 0 ){

            fscanf(fp, "%f %f %f", &x, &y, &z);

            norm.x = x;
            norm.y = y;
            norm.z = z;

            normals.push_back( norm );
            
        } else if ( strcmp(type, "f") == 0) {
            
            unsigned int vert[3], norms[3];
            // f v1//n1 v2//n2 v3//n3
            // v1, v2, v3 are the indices of the vertices and n1, n2, n3 are the indices of the normals
            fscanf(fp, "%d//%d %d//%d %d//%d", &vert[0], &norms[0], &vert[1], &norms[1], &vert[2], &norms[2]);
            
            indices.push_back( vert[0] - 1 );
            indices.push_back( vert[1] - 1 );
            indices.push_back( vert[2] - 1 );
        }
    }

    // read normal data accordingly
    fclose(fp);  
}

void OBJObject::draw(GLuint shaderProgram) 
{
    /* 
     in vertex shader:
     uniform mat4 MVP;
     uniform mat4 camera;
     uniform mat4 model;
     */
    
    // Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
    glm::mat4 MVP = Window::P * Window::V * toWorld;
    glm::mat4 model = toWorld;
    glm::mat4 camera = Window::V;
    
    // We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009),
    // gl_ModelViewProjectionMatrix has been removed from the language. The user is expected to supply
    // this matrix to the shader when using modern OpenGL.
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    GLuint CamID = glGetUniformLocation(shaderProgram, "camera");
    GLuint ModelID = glGetUniformLocation(shaderProgram, "model");
    
    // create a eye vector to reference lights from
    glm::vec3 eye = glm::vec3(0.0f, 0.0f, 20.0f);
    // pass in the camera position
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &eye[0]);
    
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(CamID, 1, GL_FALSE, &camera[0][0]);
    glUniformMatrix4fv(ModelID, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OBJObject::update()
{
    spin(1.0f);
}

void OBJObject::spin(float deg)
{
    this->angle = deg;
    if (this->angle > 360.0f || this->angle < -360.0f) 
        this->angle = 0.0f;

    // This creates the matrix to rotate the cube
    this->toWorld = this->toWorld * glm::rotate( glm::mat4(1.0f),
                                 this->angle / 180.0f * glm::pi<float>(), 
                                 glm::vec3(0.0f, 1.0f, 0.0f)) ;
}

void OBJObject::translate(float x, float y, float z){

    glm::mat4 matrix = glm::mat4(1.0f);
    this->toWorld = glm::translate(matrix, glm::vec3(x, y, z)) * this->toWorld;
}

void OBJObject::scale(float i){
    
    glm::mat4 matrix = glm::mat4(1.0f);
    this->toWorld = this->toWorld * glm::scale(matrix, glm::vec3(i, i, i)) ;
}

void OBJObject::orbit(float i){
    
    this->angle = i;
    if (this->angle > 360.0f || this->angle < -360.0f)
        this->angle = 0.0f;
    
    // This creates the matrix to rotate the cube
    this->toWorld = glm::rotate( glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(),
                                 glm::vec3(0.0f, 0.0f, 1.0f) ) * this->toWorld;
}
void OBJObject::p_size(int i){
    
    pointsize += i;
    if(pointsize < 1)
        pointsize = 1;
}
void OBJObject::reset(){
    
    glm::mat4 iMatrix = glm::mat4(1.0f);
    pointsize = 1;
    this->toWorld = iMatrix;
}

