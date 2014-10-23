#ifndef QWTCHARTSETDIALOG_H
#define QWTCHARTSETDIALOG_H

#include <QWidget>
#include "qttreepropertybrowser.h"
#include <QtWidgets/QHBoxLayout>
#include "chartwave_qwt.h"
#include "qtpropertymanager.h"
#include <QtGroupPropertyManager>
#include <QtVariantPropertyManager>
#include <QtVariantEditorFactory>
#include <QtColorPropertyManager>
#include <QList>
#include <QMap>
#include "qtpropertyidstorage.h"
#include <string>
using std::string;
class QwtChartSetDialog : public QWidget
{
	Q_OBJECT

public:
	QwtChartSetDialog(QWidget *parent,ChartWave_qwt* plot);
	~QwtChartSetDialog();

private:
	ChartWave_qwt *m_plot;
	QtTreePropertyBrowser *m_property;
	QHBoxLayout *m_mainHorizontalLayout;
	QtGroupPropertyManager* m_propertyGroup;
	QtVariantPropertyManager *m_variantManager;
	QtVariantEditorFactory *m_variantFactory;
	QtPropertyIdStorage<string> m_property_id;
	QMap<QtVariantProperty*,QwtPlotCurve*> m_property_curve;
	void createUI();
	void addPlotSet(ChartWave_qwt* plot);
	void addCurveSet(ChartWave_qwt* plot);
	void addCurveSet(QwtPlotCurve* curve);
	QList<string> m_curIDs;
private slots:
	void onPropertyValueChanged(QtProperty * property, const QVariant & value);
	QwtPlotCurve* getCurvePtr(QtProperty * property);
	static QString penStyle2Chinese(const Qt::PenStyle penStyle);
	static Qt::PenStyle Chinese2PenStyle(const QString str);
};

#endif // QWTCHARTSETDIALOG_H
