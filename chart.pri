
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include($$PWD/qwt_set.pri)

HEADERS += \
    $$PWD/chartwave_qwt.h \
    $$PWD/QwtChartSetDialog.h \
    $$PWD/CurveSelectDialog.h \
    $$PWD/SAPlotChart.h
SOURCES += \
    $$PWD/chartwave_qwt.cpp \
    $$PWD/QwtChartSetDialog.cpp \
    $$PWD/CurveSelectDialog.cpp \
    $$PWD/SAPlotChart.cpp
