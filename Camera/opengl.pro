QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    main.cpp \
    mainwindow.cpp \
    opengl.cpp

HEADERS += \
    camera.h \
    mainwindow.h \
    opengl.h

FORMS += \
    mainwindow.ui


LIBS += -L$$PWD/openGL/ -llibassimp.dll

INCLUDEPATH += D:/C++/openGL/assimp-master/include
#INCLUDEPATH += $$PWD/openGL
DEPENDPATH += $$PWD/openGL

RESOURCES += \
    res.qrc
