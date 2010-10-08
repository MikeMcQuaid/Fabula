QT += sql
TARGET = Fabula
TEMPLATE = app
CONFIG += precompile_header \
    embed_manifest_exe
DEFINES += QT_NO_CAST_TO_ASCII

SOURCES += main.cpp \
    MainWindow.cpp \
    Database.cpp \
    TableTreeModel.cpp \
    PreferencesDialog.cpp \
    EventDialog.cpp \
    EventDelegate.cpp
HEADERS += MainWindow.h \
    Database.h \
    TableTreeModel.h \
    PreferencesDialog.h \
    EventDialog.h \
    EventDelegate.h
FORMS += MainWindow.ui \
    PreferencesDialog.ui \
    EventDialog.ui
RESOURCES += Fabula.qrc

debug {
    CONFIG += warn_on
}

win32 {
    CONFIG += embed_manifest_exe
}

!win32-msvc {
    QMAKE_CXXFLAGS += -Wall -Werror -Wextra
        -Wold-style-cast -Wformat=2 -Winit-self \
        -Wswitch-default -Wundef -Wpointer-arith \
        -Wcast-qual -Wconversion -Wunused \
        -Wmissing-format-attribute -Wredundant-decls \
        -Winline -Wnon-virtual-dtor -Wsign-promo \
        -Woverloaded-virtual -Wstrict-null-sentinel \
        -Wno-pmf-conversions -Wctor-dtor-privacy
}

macx {
    RC_FILE = icons/audio-input-microphone.icns
    release {
        ## TODO: multiarch release builds
        #CONFIG += x86 x86_64 ppc
    }
}
