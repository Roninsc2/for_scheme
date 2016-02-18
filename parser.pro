TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    lexer.cpp \
    parser.cpp \
    byte_code_gen.cpp \
    stack.cpp \
    byte_code_cmd.cpp \
    scheme_std_lib.cpp \
    vlist.cpp \
    type.cpp

HEADERS += \
    lexer.h \
    parser.h \
    ast.h \
    texeption.h \
    byte_code_gen.h \
    stack.h \
    byte_code_cmd.h \
    vlist.h \
    type.h \
    saveload.h

