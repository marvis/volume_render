VPATH += ../shared
INCLUDEPATH += ../shared

HEADERS       = glwidget.h \
                window.h \
                qtlogo.h \
				vsvr.h \
				tiff_io.h
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                qtlogo.cpp \
				vsvr.cpp \
				tiff_io.cpp
LIBS += -ltiff -lGLEW
QT           += opengl

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS hellogl.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
