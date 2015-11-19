TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    lexer.cpp \
    parser.cpp \
    byte_code.cpp

HEADERS += \
    lexer.h \
    parser.h \
    ast.h \
    byte_code.h \
    texeption.h

