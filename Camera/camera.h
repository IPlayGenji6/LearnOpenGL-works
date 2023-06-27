#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>
#include <QEvent>
#include <QMouseEvent>

class Camera
{
public:
    Camera(QWidget *widget);

    void init();
    float getMoveSpeed() const;
    void setMoveSpeed(float value);

    void eventHandle(QEvent *event);
    QMatrix4x4 getView() const;

private:


private:
    QWidget *widget;
    //鼠标
    bool isRightButtonPressed = false;
    bool isLeftButtonPressed = false;
    QPoint lastPos;

    QVector3D cameraPos;        //摄像机位置
    QVector3D modelPos;         //观察物位置
    QVector3D lookAtUp;         //摄像机上方向坐标轴
    QVector3D cameraDirection;  //摄像机观察方向向量
    QVector3D cameraRight;      //摄像机右向量
    QVector3D cameraRightRef;
    QVector3D cameraUp;         //摄像机上向量

    float moveSpeed;            //控制移动速度
    float scaleFactor;

    QMatrix4x4 view;            //观察矩阵
};

#endif // CAMERA_H
