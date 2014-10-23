
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
#qwt set
QWT_ROOT = $$PWD/qwt-6.1.0


include( $${QWT_ROOT}/qwtfunctions.pri )
CONFIG += qwt
QWT_CONFIG  += QwtDll

INCLUDEPATH += $${QWT_ROOT}/src
DEPENDPATH  += $${QWT_ROOT}/src
contains(QWT_CONFIG, QwtFramework) {

    LIBS      += -F$${QWT_ROOT}/lib
}
else {
    LIBS      += -L$${QWT_ROOT}/lib
}
qwtAddLibrary(qwt)

contains(QWT_CONFIG, QwtOpenGL ) {

    QT += opengl
}
else {

    DEFINES += QWT_NO_OPENGL
}
contains(QWT_CONFIG, QwtSvg) {

    QT += svg
}
else {

    DEFINES += QWT_NO_SVG
}
win32 {
    contains(QWT_CONFIG, QwtDll) {
        DEFINES    += QT_DLL QWT_DLL
    }
}
#end qwt set
