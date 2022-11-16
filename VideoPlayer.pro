QT       += core gui multimedia multimediawidgets sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    current_playlist.cpp \
    favorite.cpp \
    jumpto.cpp \
    main.cpp \
    mainwindow.cpp \
    mythread.cpp \
    mythread1.cpp \
    urlplayer.cpp \
    videomessage.cpp

HEADERS += \
    current_playlist.h \
    favorite.h \
    jumpto.h \
    mainwindow.h \
    mythread.h \
    mythread1.h \
    urlplayer.h \
    videomessage.h

FORMS += \
    current_playlist.ui \
    favorite.ui \
    jumpto.ui \
    mainwindow.ui \
    urlplayer.ui \
    videomessage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../OpenCV-4.5.5/build/x64/vc16/lib/ -lopencv_world455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../OpenCV-4.5.5/build/x64/vc16/lib/ -lopencv_world455d
else:unix: LIBS += -L$$PWD/../../../../OpenCV-4.5.5/build/x64/vc16/lib/ -lopencv_world455

INCLUDEPATH += $$PWD/../../../../OpenCV-4.5.5/build/include
DEPENDPATH += $$PWD/../../../../OpenCV-4.5.5/build/include
