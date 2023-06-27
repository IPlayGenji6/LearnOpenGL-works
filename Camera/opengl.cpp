#include "opengl.h"

OpenGL::OpenGL(QWidget *parent)
    : QOpenGLWidget(parent)
    , camera(this)
{
    loadModel( "C:/Users/71058/Downloads/f18_blender/scene.gltf");
    //MSAA
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4); // 设置4倍抗锯齿
    setFormat(format);

    connect(&timer, &QTimer::timeout, [=]{
        update();
    });
    timer.start(10);

    setCursor(Qt::OpenHandCursor);
}

OpenGL::~OpenGL()
{
    if(!isValid())
        return;
    makeCurrent();
    vao.destroy();
    vbo.destroy();
    ebo.destroy();
    doneCurrent();
}

void OpenGL::loadModel(const QString &filepath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath.toUtf8(), aiProcess_Triangulate |
                                             aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode) {
        qDebug()<<"func:"<<__FUNCTION__<<importer.GetErrorString();
        return;
    }else{
        // 以递归方式处理ASSIMP的根节点
        processNode(scene->mRootNode, scene);
    }    
}

void OpenGL::processNode(aiNode *node, const aiScene *scene)
{
//    qDebug()<<"func:"<<__FUNCTION__<<"mNumMeshes:"<<node->mNumMeshes<<"mNumChildren:"<<node->mNumChildren;
    // 处理位于当前节点的每个网格
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // 节点对象仅包含索引用来索引场景中的实际对象。
        // 场景包含所有数据，节点只是为了有组织的保存东西（如节点之间的关系）。
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // 在我们处理完所有网格（如果有的话）后，我们会递归处理每个子节点
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }    
}

void OpenGL::processMesh(aiMesh *mesh, const aiScene *scene)
{    
    Q_UNUSED(scene)
//    qDebug()<<"func:"<<__FUNCTION__<<"mNumVertices:"<<mesh->mNumVertices<<"mNumFaces:"<<mesh->mNumFaces;
    // 简单的只取顶点坐标和索引

    MeshMaterial meshMat;

    // 遍历每个网格的顶点
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        //位置
        position << mesh->mVertices[i].x << mesh->mVertices[i].y << mesh->mVertices[i].z;
        //纹理坐标
        if (mesh->HasTextureCoords(0))
        {
            textureCoord << mesh->mTextureCoords[0][i].x << mesh->mTextureCoords[0][i].y;
        }
        //法线
        {
            normal << mesh->mNormals[i].x << mesh->mNormals[i].y << mesh->mNormals[i].z;
        }
    }

    //该网格的材质
    int matIdx = mesh->mMaterialIndex;
    aiColor3D baseColor;
    scene->mMaterials[matIdx]->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
    meshMat.diffuseColor = QVector3D(baseColor.r, baseColor.g, baseColor.b);

    // 现在遍历每个网格面（一个面是一个三角形的网格）并检索相应的顶点索引。
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
//        if(i == 0) qDebug()<<"face.mNumIndices:"<<face.mNumIndices;
//         检索面的所有索引并将它们存储在索引向量中
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j] + indicesOffset);
            meshMat.numIndices++;
        }
    }

    indicesOffset += mesh->mNumVertices;
    meshMaterial.push_back(meshMat);
}

void OpenGL::initializeGL()
{
    initializeOpenGLFunctions();

    //顶点着色器
    bool success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/plane.vert");
    if(!success){
        qDebug()<<"compiler vertex failed!"<<shaderProgram.log();
    }
    //片段着色器
    success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/plane.frag");
    if(!success){
        qDebug()<<"compiler fragment failed!"<<shaderProgram.log();
    }
    success = shaderProgram.link();
    if(!success){
        qDebug()<<"link shader failed!"<<shaderProgram.log();
    }

    vao.create();
    vao.bind();

    vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo.create();
    vbo.bind();
    QVector<float> combinedData;
    for (int i = 0; i < position.size() / 3; i++)
    {
        combinedData.append(position[i*3]);
        combinedData.append(position[i*3 + 1]);
        combinedData.append(position[i*3 + 2]);
        combinedData.append(normal[i*3]);
        combinedData.append(normal[i*3 + 1]);
        combinedData.append(normal[i*3 + 2]);
//        combinedData.append(textureCoord[i*2]);
//        combinedData.append(textureCoord[i*2 +1]);
    }
    vbo.allocate(combinedData.data(), combinedData.size() * sizeof(float)); //顶点数据

    ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    ebo.create();
    ebo.bind();
    ebo.allocate(indices.data(), indices.size() * sizeof(int)); //索引数据

    //(int location, GLenum type, int offset, int tupleSize, int stride = 0);
    shaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);
    shaderProgram.enableAttributeArray(0);
    shaderProgram.setAttributeBuffer(1, GL_FLOAT, 3*sizeof(float), 3, sizeof(GLfloat) * 6);
    shaderProgram.enableAttributeArray(1);
//    shaderProgram.setAttributeBuffer(2, GL_FLOAT, 6*sizeof(float), 2, sizeof(GLfloat) * 8);
//    shaderProgram.enableAttributeArray(2);

    vao.release();
    vbo.release();
    ebo.release();

    glEnable(GL_DEPTH_TEST);
}

void OpenGL::paintGL()
{
    //一般来说，在渲染函数中，需要执行以下几个步骤：
    //1.绑定需要渲染的着色器程序：使用glUseProgram()函数，将需要使用的着色器程序绑定到OpenGL的渲染管线中
    //2.设置uniform变量的值：使用glUniform*()系列函数，设置需要在shader程序中使用的uniform变量的值
    //3.绑定VAO：使用glBindVertexArray()函数，绑定需要渲染的VAO，确保OpenGL使用正确的顶点属性配置
    //4.调用渲染函数：使用glDraw*()系列函数，绘制物体
    //5.解绑VAO：使用glBindVertexArray(0)函数，解绑VAO
    //6.解绑着色器程序：使用glUseProgram(0)函数，解绑着色器程序
    //需要注意的是，在绑定VAO和调用渲染函数之前，需要确保所有需要绑定的VBO和EBO都已经被正确地绑定到了VAO中。另外，如果在渲染函数中使用了多个VAO，则需要依次绑定和解绑每个VAO
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT, GL_FILL); // 填充模式
    shaderProgram.bind();
    {
        QMatrix4x4 model; //模型矩阵
//        model.rotate(angle, QVector3D(0.0f, 0.0f, 1.0f));
        shaderProgram.setUniformValue("model", model);        
        shaderProgram.setUniformValue("view", camera.getView());
        QMatrix4x4 projection; //透视投影
        projection.perspective(60.0f, 1.0f * width() / height(), 0.1f, 600.0f);
        shaderProgram.setUniformValue("projection", projection);
        QOpenGLVertexArrayObject::Binder vao_bind(&vao); Q_UNUSED(vao_bind);//作用域的开头和结束时自动绑定和解绑
        for (int i = 0, offset = 0; i < meshMaterial.size(); offset += meshMaterial[i].numIndices, i++)
        {
            shaderProgram.setUniformValue("diffuseColor", meshMaterial[i].diffuseColor);
            glDrawElements(GL_TRIANGLES, meshMaterial[i].numIndices, GL_UNSIGNED_INT, (void*)(offset * sizeof(uint)));
        }
//        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    shaderProgram.release();
}

void OpenGL::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 * @brief 将父类的事件(鼠标事件)传递给camera处理
 * @param event
 * @return
 */
bool OpenGL::event(QEvent *event)
{
    camera.eventHandle(event);
    return QWidget::event(event);
}


/*std::vector<QOpenGLTexture*> textures;
for (unsigned int i = 0; i < scene->mNumTextures; i++) {
    aiTexture* texture = scene->mTextures[i];
    QImage image;
    image.load(QString::fromStdString(texture->mFilename.C_Str()));

    QOpenGLTexture* glTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    glTexture->create();
    glTexture->setData(image.mirrored());
    glTexture->setWrapMode(QOpenGLTexture::Repeat);
    glTexture->setMinificationFilter(QOpenGLTexture::Linear);
    glTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    textures.push_back(glTexture);
}*/
