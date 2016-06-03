//
//  WavefrontSection.cpp
//  Final
//
//  Created by Joe Lu Wu on 6/3/16.
//  Copyright © 2016 Joe Lu Wu. All rights reserved.
//

#include "WavefrontSection.hpp"

int WavefrontModel::load(string file,string _mtlFile)
{
    gl3WriteOnStream("Loading model "+file,log_stream);
    
    ifstream model(file.c_str());
    
    if(!model)
    {
        cerr<<"Cannot open file "<<file<<endl;
        gl3WriteOnStream("Cannot open file "+file,error_stream);
        return -1;
    }
    
    char buf[256];
    char *curr_tok = NULL;
    
    /* These vectors stores all the vertices,normals and texture coord.
     Later each section will just copy in their arrays their vertices, normals
     and coords. To get the right order, i store the .obj face index information
     in WavefrontSections::*_index vector */
    
    std::vector<float> vert;
    std::vector<float> norm;
    std::vector<float> tex;
    
    mtlHeader.Open(_mtlFile);  /* I open the material file, to parse all the materials */
    
    int curr_section = 0;
    
    while(!model.eof())
    {
        model.getline(buf,256);
        
        curr_tok = strtok(buf,WHITESPACE);
        
        if(!curr_tok || curr_tok[0] == '#')
        {
            continue;
        }
        else if(strcmp(curr_tok,"v") == 0)
        {
            vert.push_back(atof(strtok(NULL,WHITESPACE)));
            vert.push_back(atof(strtok(NULL,WHITESPACE)));
            vert.push_back(atof(strtok(NULL,WHITESPACE)));
        }
        else if(strcmp(curr_tok,"vn") == 0)
        {
            norm.push_back(atof(strtok(NULL,WHITESPACE)));
            norm.push_back(atof(strtok(NULL,WHITESPACE)));
            norm.push_back(atof(strtok(NULL,WHITESPACE)));
        }
        else if(strcmp(curr_tok,"vt") == 0)
        {
            tex.push_back(atof(strtok(NULL,WHITESPACE)));
            tex.push_back(atof(strtok(NULL,WHITESPACE)));
        }
        else if(strcmp(curr_tok,"usemtl") == 0)
        {
            char *mtl = strtok(NULL,WHITESPACE);
            bool already_pres = false;
            
            /*I check if a sections already uses that material,
             if so, i add the new indexes to that section,
             otherwise I just create a new section */
            
            for(int i = 0;i < sections.size();i++)
            {
                if(strcmp(mtl,sections[i]->material->name.c_str()) == 0)
                {
                    curr_section = i;
                    already_pres = true;
                    break;
                }
            }
            
            
            if(!already_pres)
            {
                curr_section = sections.size();
                sections.push_back(new WavefrontSection(mtl));
                sections[curr_section]->material = mtlHeader.parseMaterial(mtl);
            }
        }
        else if(strcmp(curr_tok,"f") == 0)
        {
            char *tmp_tok = NULL;
            char *temp = NULL;
            
            /* Here I parse model's faces. I added a '-1' to the indexes, because in the .obj
             file they start from 1, but in my vectors they start from 0 */
            
            while((tmp_tok = strtok(NULL,WHITESPACE)))
            {
                sections[curr_section]->vert_index.push_back(atoi(tmp_tok)-1);
                
                if((temp = strstr(tmp_tok,"//"))) ///no texture
                {
                    temp++;
                    sections[curr_section]->norm_index.push_back(atoi(++temp)-1);
                }
                else if((temp = strstr(tmp_tok,"/"))) ///with texture
                {
                    sections[curr_section]->tex_index.push_back(atoi(++temp)-1);
                    
                    if((temp = strstr(temp,"/")))
                    {
                        sections[curr_section]->norm_index.push_back(atoi(++temp)-1);
                    }
                }
            }
        }
        
    }
    
    
    cout<<"This model has got "<<sections.size()<<" sections"<<endl;
    
    /* Now I add vertices, normals and texture coords to each section,
     using the indexes that I previously stored in each section */
    
    for(int i = 0;i < sections.size();i++)
    {
        for(int j = 0;j < sections[i]->vert_index.size();j++)
        {
            sections[i]->vert.push_back(vert[sections[i]->vert_index[j]*3]);   //x component
            sections[i]->vert.push_back(vert[sections[i]->vert_index[j]*3+1]); //y component
            sections[i]->vert.push_back(vert[sections[i]->vert_index[j]*3+2]); //z component
        }
        
        for(int j = 0;j < sections[i]->tex_index.size();j++)
        {
            sections[i]->tex_coord.push_back(tex[sections[i]->tex_index[j]*2]);   //u component
            sections[i]->tex_coord.push_back(tex[sections[i]->tex_index[j]*2+1]); //v component
        }
        
        for(int j = 0;j < sections[i]->norm_index.size();j++)
        {
            sections[i]->norm.push_back(norm[sections[i]->norm_index[j]*3]);     //x component
            sections[i]->norm.push_back(norm[sections[i]->norm_index[j]*3+1]);   //y component
            sections[i]->norm.push_back(norm[sections[i]->norm_index[j]*3+2]);   //z component
        }
        
        sections[i]->ConstructVAO();   //I construct the VAO for each section
        
        printf("Section %i has %i verts, %i normals, %i tex coords\n",i,sections[i]->vert.size()/3,sections[i]->norm.size()/3,sections[i]->tex_coord.size()/2);
    }
    
    
}
