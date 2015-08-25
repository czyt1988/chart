
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include($$PWD/qwt_set.pri)

HEADERS += \
    $$PWD/chartwave_qwt.h \
    $$PWD/QwtPlotPropertySetDialog.h \
    $$PWD/CurveSelectDialog.h

SOURCES += \
    $$PWD/chartwave_qwt.cpp \
    $$PWD/QwtPlotPropertySetDialog.cpp \
    $$PWD/CurveSelectDialog.cpp

#FORMS += \
#    $$PWD/QwtPlotSetWidget.ui
