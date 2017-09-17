#-------------------------------------------------
#
# Project created by QtCreator 2017-08-20T21:03:45
#
#-------------------------------------------------

QT       -= core gui

TARGET = codex
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH +=  ./../../../codex/include/

CODEX_INCLUDE_DIR = ./../../../codex/include/codex
CODEX_SRCS_DIR = ./../../../codex/srcs

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


HEADERS += \
        $$CODEX_INCLUDE_DIR/codex.hpp\
        $$CODEX_INCLUDE_DIR/error_code.hpp\
        $$CODEX_INCLUDE_DIR/function.hpp\
        $$CODEX_INCLUDE_DIR/function.ipp\
        $$CODEX_INCLUDE_DIR/vision/image.hpp\
        $$CODEX_INCLUDE_DIR/vision/bitmap.hpp\
        $$CODEX_INCLUDE_DIR/vision/vision.hpp\
        $$CODEX_INCLUDE_DIR/vision/kernel.hpp\
        $$CODEX_INCLUDE_DIR/vision/image_proc.hpp\
        $$CODEX_INCLUDE_DIR/vision/image_draw.hpp\
        $$CODEX_INCLUDE_DIR/vision/fft.hpp\
        $$CODEX_INCLUDE_DIR/vision/sobel.hpp\
        $$CODEX_INCLUDE_DIR/vision/canny.hpp\
        $$CODEX_INCLUDE_DIR/vision/hough.hpp\
        $$CODEX_INCLUDE_DIR/vision/harris.hpp

SOURCES += \
        $$CODEX_SRCS_DIR/codex.cpp\
        $$CODEX_SRCS_DIR/error_code.cpp\
        $$CODEX_SRCS_DIR/vision/image.cpp\
        $$CODEX_SRCS_DIR/vision/bitmap.cpp\
        $$CODEX_SRCS_DIR/vision/image_proc.cpp\
        $$CODEX_SRCS_DIR/vision/image_draw.cpp\
        $$CODEX_SRCS_DIR/vision/fft.cpp\
        $$CODEX_SRCS_DIR/vision/canny.cpp\
        $$CODEX_SRCS_DIR/vision/hough.cpp\
        $$CODEX_SRCS_DIR/vision/harris.cpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
