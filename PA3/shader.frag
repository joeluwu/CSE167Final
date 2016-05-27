#version 330 core

// This is a fragment shader.

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 intensities; //a.k.a the color of the light
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    bool on;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} ;

struct PointLight {
    bool on;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    bool on;
    
    vec3 position;
    vec3 direction;
    float cutOff;
    float exponent;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


//uniform mat4 model;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

in vec3 fragNormal;
in vec3 fragVert;
out vec4 finalColor;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // Properties
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragVert);
    vec3 result = norm;
  
    // ========================================================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // ========================================================================================

    if (pointLight.on)
        result = CalcPointLight(pointLight, norm, fragVert, viewDir);
    
    if (dirLight.on)
        result = CalcDirLight(dirLight, norm, viewDir);
    
    if(spotLight.on)
        result = CalcSpotLight(spotLight, norm, fragVert, viewDir);
    
    finalColor = vec4(result, 1.0);
}


// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // direction of the light
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse shading
    float diff = max( dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = normalize( reflect(-lightDir, normal) );
    float spec = pow( max(dot(viewDir, reflectDir), 0.0 ), material.shininess);
    
    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    
    //return -vec3(dot(light.direction, normal),dot(light.direction, normal),dot(light.direction, normal));
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (0.001 * (distance * distance));
    
    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Diffuse shading
    float diff = max( dot(normal, lightDir), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = ( pow( dot(-light.position, -lightDir), light.exponent)  ) / (0.001 * (distance * distance));
    //float attenuation = ( pow( dot(-light.position, -lightDir), 0.01)  ) / (0.001 * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
 
    diffuse *= attenuation;
    specular *= attenuation ;
    
    vec3 pos_norm = normalize( -light.position );
    vec3 dir_norm = normalize( -lightDir );
    
    if( acos(dot(pos_norm, dir_norm)) <= light.cutOff )
        return (ambient + diffuse + specular);
    else
        return ambient;
}

