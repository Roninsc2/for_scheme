TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    lexer.cpp \
    parser.cpp \
    vlist.cpp

HEADERS += \
    lexer.h \
    parser.h \
    vlist.h \
    ast.h

