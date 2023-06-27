#include "camera.h"

Camera::Camera(QWidget *widget)
    : widget(widget)
    , cameraPos(QVector3D(20, 0, 20))
    , modelPos(QVector3D(0, 0, 0))
    , lookAtUp(QVector3D(0, 0, 1))
    , moveSpeed(0.05)
    , scaleFactor(0.01)
{
    init();
}

void Camera::init()
{

    view.setToIdentity();
    view.lookAt(cameraPos, modelPos, lookAtUp);
    cameraDirection = (modelPos - cameraPos).normalized();
    cameraRight = QVector3D::crossProduct(cameraDirection, lookAtUp).normalized();
    cameraRightRef = cameraRight;
    cameraUp = QVector3D::crossProduct(cameraRight, cameraDirection).normalized();
    QMatrix4x4 a;
    a.rotate(45, QVector3D(0,0,1));
    qDebug()<<a * cameraPos;
}

float Camera::getMoveSpeed() const
{
    return moveSpeed;
}

void Camera::setMoveSpeed(float value)
{
    moveSpeed = value;
}

void Camera::eventHandle(QEvent *event)
{    
    if (event->type() == QEvent::MouseButtonPress)
    {
        auto e = static_cast<QMouseEvent*>(event);
        lastPos = e->pos();
        widget->setCursor(Qt::ClosedHandCursor);
        if (e->button() == Qt::LeftButton)
        {
            isRightButtonPressed = false;
            isLeftButtonPressed = true;
        }
        else if (e->button() == Qt::RightButton)
        {
            isLeftButtonPressed = false;
            isRightButtonPressed = true;
        }

    }
    else if (event->type() == QEvent::MouseMove)
    {
        auto e = static_cast<QMouseEvent*>(event);
        QPointF delta = e->pos() - lastPos;
        qreal dx = delta.x() * moveSpeed;
        qreal dy = delta.y() * moveSpeed;
        if (isLeftButtonPressed)        //左键旋转
        {
            dx = -10*dx;
            dy = -10*dy;
            QMatrix4x4 rotateMat;
            rotateMat.rotate(dx, lookAtUp);
            if (dy > 0)
            {
                if (QVector3D::dotProduct(cameraDirection, lookAtUp) < 0.999)
                    rotateMat.rotate(dy, cameraRight);
            }
            else
            {
                if (QVector3D::dotProduct(cameraDirection, lookAtUp) > -0.999)
                    rotateMat.rotate(dy, cameraRight);
            }

            QVector3D tmpCameraPos = cameraPos;
            QVector3D tmpModelPos = modelPos;
            cameraPos = rotateMat * cameraPos;
            modelPos = rotateMat * modelPos;            
            cameraDirection = (modelPos - cameraPos).normalized();
            if (qAbs(QVector3D::dotProduct(cameraDirection, lookAtUp)) < 0.999) //cameraDirection与lookAtUp平行
            {
                cameraRight = QVector3D::crossProduct(cameraDirection, lookAtUp).normalized();
                if (QVector3D::dotProduct(cameraRight, cameraRightRef) < 0)
                {
                    cameraRight = cameraRightRef;
                    cameraPos = tmpCameraPos;
                    modelPos = tmpModelPos;
                }
                cameraRightRef = cameraRight;
            }
            cameraUp = QVector3D::crossProduct(cameraRight, cameraDirection).normalized();
//            qDebug()<<"cameraPos:("<<cameraPos.x()<<cameraPos.y()<<cameraPos.z()<<")"
//                    <<"cameraDirection:("<<cameraDirection.x()<<cameraDirection.y()<<cameraDirection.z()<<")"
//                    <<"cameraRight:("<<cameraRight.x()<<cameraRight.y()<<cameraRight.z()<<")"
//                    <<"cameraUp:("<<cameraUp.x()<<cameraUp.y()<<cameraUp.z()<<")";
            view.setToIdentity();
            view.lookAt(cameraPos, modelPos, lookAtUp);

        }
        else if (isRightButtonPressed)    //右键拖拽平移摄像机
        {
            cameraPos = cameraPos - dx * cameraRight + dy * cameraUp;
            modelPos = modelPos - dx * cameraRight + dy * cameraUp;
            view.setToIdentity();
            view.lookAt(cameraPos, modelPos, lookAtUp);
        }

        lastPos = e->pos();
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        auto e = static_cast<QMouseEvent*>(event);
        widget->setCursor(Qt::OpenHandCursor);
        if (e->button() == Qt::LeftButton)
        {
            isLeftButtonPressed = false;
        }
        else if (e->button() == Qt::RightButton)
        {
            isRightButtonPressed = false;
        }
    }
    else if (event->type() == QEvent::Wheel)
    {
        auto e = static_cast<QWheelEvent*>(event);
        qreal dy = e->angleDelta().y() * scaleFactor;
        cameraPos = cameraPos + dy * cameraDirection;
        view.setToIdentity();
        view.lookAt(cameraPos, modelPos, lookAtUp);
    }
}

QMatrix4x4 Camera::getView() const
{
    return view;
}

