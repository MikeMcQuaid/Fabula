# -------------------------------------------------
# Project created by QtCreator 2010-01-25T18:12:04
# -------------------------------------------------
QT += sql \
    xml \
    phonon
TARGET = Fabula
TEMPLATE = app
SOURCES += main.cpp \
    MainWindow.cpp \
    Database.cpp \
    TreeTableProxyModel.cpp
HEADERS += MainWindow.h \
    Database.h \
    TreeTableProxyModel.h
FORMS += MainWindow.ui
CONFIG += warn_on precompile_header embed_manifest_exe
#DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
#QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
#CONFIG+=x86 x86_64 ppc ppc64
QMAKE_CXXFLAGS += -Wall -Wextra -Werror
