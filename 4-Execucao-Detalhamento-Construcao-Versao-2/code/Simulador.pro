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
    Main.cpp \
    src/Model/CCalculadoraFluxoFracionario.cpp \
    src/Model/CCelula.cpp \
    src/Model/CCurvasPermeabilidadeChierici.cpp \
    src/Model/CCurvasPermeabilidadeCorey.cpp \
    src/Model/CCurvasPermeabilidadeLET.cpp \
    src/Model/CCurvasPermeabilidadeTabelada.cpp \
    src/Model/CMalha.cpp \
    src/Model/CRelatorio.cpp \
    src/Model/CSimulador.cpp \
    src/Model/CSolver.cpp \
    src/Model/CWelge.cpp \
    src/View/Mainwindow.cpp \
    src/View/QCustomplot.cpp

# --- CABEÇALHOS (.H) ---
HEADERS += \
    src/Model/CCalculadoraFluxoFracionario.h \
    src/Model/CCelula.h \
    src/Model/CCurvasPermeabilidadeChierici.h \
    src/Model/CCurvasPermeabilidadeCorey.h \
    src/Model/CCurvasPermeabilidadeLET.h \
    src/Model/CCurvasPermeabilidadeTabelada.h \
    src/Model/CMalha.h \
    src/Model/CRelatorio.h \
    src/Model/CSimulador.h \
    src/Model/CSolver.h \
    src/Model/CWelge.h \
    src/Model/ICurvasPermeabilidade.h \
    src/View/Mainwindow.h \
    src/View/QCustomplot.h

# --- INTERFACE (.UI) ---
FORMS += \
    src/View/Mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/View/recursos.qrc
