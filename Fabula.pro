QT += sql
TARGET = Fabula
TEMPLATE = app
CONFIG += precompile_header \
    embed_manifest_exe
DEFINES += QT_NO_CAST_TO_ASCII

SOURCES += main.cpp \
    MainWindow.cpp \
    Database.cpp \
    TableTreeModel.cpp
HEADERS += MainWindow.h \
    Database.h \
    TableTreeModel.h
FORMS += MainWindow.ui
RESOURCES += Fabula.qrc

debug {
    CONFIG += warn_on
}

win32 {
    CONFIG += embed_manifest_exe
}

!win32-msvc {
    QMAKE_CXXFLAGS += -Wextra \
        -Werror
}

macx {
    RC_FILE = icons/audio-input-microphone.icns
    release {
        ## TODO: multiarch release builds
        #CONFIG += x86 x86_64 ppc
    }
}
