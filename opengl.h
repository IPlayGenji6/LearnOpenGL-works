#ifndef OPENGL_H
#define OPENGL_H

#include <QCoreApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QTimer>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include "camera.h"

struct MeshMaterial
{
    quint32 numIndices;

    QVector3D ambient;
    QVector3D diffuseColor;
    QVector3D specular;
    float shininess;

    MeshMaterial() : numIndices(0), diffuseColor(QVector3D(0.0f, 0.0f, 0.0f)){}
};

class OpenGL : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    OpenGL(QWidget *parent = nullptr);
    ~OpenGL();

    // 加载模型
    void loadModel(const QString &filepath);
    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *mesh, const aiScene *scene);

protected:
    // 渲染需要实现的三个虚函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    bool event(QEvent *event) override;

private:
    QTimer timer;

    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ebo;

    Camera camera;

    // 存顶点和索引
    QVector<float> position;
    QVector<float> normal;
    QVector<float> textureCoord;
    QVector<unsigned int> indices;
    int indicesOffset = 0;
    QVector<MeshMaterial> meshMaterial;

    QMatrix4x4 view;
};

#endif // OPENGL_H
