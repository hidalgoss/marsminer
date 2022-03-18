QT       += core gui

#QT += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    marsminermainwin.cpp \
    MarsMiner.cpp \
    Attribute.cpp \
    Cluster.cpp

HEADERS += \
    marsminermainwin.h \
    MarsMiner.h \
    Attribute.h \
    Cluster.h

FORMS += \
    marsminermainwin.ui


RESOURCES += \
    MarsMinerResource.qrc

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
qnx: target.path = /home/jose/hss/hss/dev/qt/mm/$${TARGET}/bin
else: unix:!android: target.path = /home/jose/hss/hss/dev/qt/mm/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
