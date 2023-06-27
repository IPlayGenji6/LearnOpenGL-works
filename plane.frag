#version 330 core
in vec2 texCroods;
in vec3 normal;
in vec3 fragPos;
in vec3 viewPos;

out vec4 fragColor;

uniform sampler2D textureBase;
uniform sampler2D textureMetallicRoughness;
uniform sampler2D textureNormal;
uniform vec3 diffuseColor;

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 0.78);
    vec3 lightPos = vec3(-20000, 20000, 15000);

    //ambient
    vec3 ambient = 0.5 * vec3(1.0, 1.0, 1.0) * diffuseColor.rgb;

    //diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);//视线方向向量点乘反射光方向向量⌈确保不是负值，介于0~1之间，幂次越大，反射向量与观察方向的角度差越大则反光效果越弱⌋
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) ;

    fragColor = vec4(result, 1.0);
}
