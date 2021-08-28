TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        api.c \
        dumper.c \
        emitter.c \
        loader.c \
        main.cpp \
        parser.c \
        reader.c \
        scanner.c \
        writer.c

HEADERS += \
    yaml.h \
    yaml2ex.hpp \
    yaml_private.h
