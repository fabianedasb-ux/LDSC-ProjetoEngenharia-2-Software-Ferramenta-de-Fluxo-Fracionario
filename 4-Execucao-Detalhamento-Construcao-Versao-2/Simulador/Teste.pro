# --- ARQUIVO .PRO ATUALIZADO ---

QT       += core gui
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
DEFINES += QT_DEPRECATED_WARNINGS

# Caminhos para o compilador achar os .h
INCLUDEPATH += src/Model \
               src/View

# --- FONTES (C++) ---
SOURCES += \
    main.cpp \
    src/Model/ccelula.cpp \
    src/Model/cmalha.cpp \
    src/Model/csimulador.cpp \
    src/Model/csolver.cpp \
    src/View/mainwindow.cpp \
    src/View/qcustomplot.cpp

# --- CABEÇALHOS (.H) ---
HEADERS += \
    src/Model/ccelula.h \
    src/Model/cmalha.h \
    src/Model/csimulador.h \
    src/Model/csolver.h \
    src/View/mainwindow.h \
    src/View/qcustomplot.h

# --- INTERFACE (.UI) ---
FORMS += \
    src/View/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
