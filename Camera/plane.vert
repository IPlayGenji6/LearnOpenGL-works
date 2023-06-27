#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCroods;
out vec3 normal;
out vec3 fragPos;
out vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCroods = aTexCoords;
    normal = mat3(transpose(inverse(model))) * aNormal;//法线矩阵被定义为「模型矩阵左上角3x3部分的逆矩阵的转置矩阵」
    fragPos = vec3(model * vec4(aPos, 1.0));
    viewPos = vec3(view);
}
