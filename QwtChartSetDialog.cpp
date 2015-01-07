#include "QwtChartSetDialog.h"
#include <QBrush>
#include <QPen>
#define PROPERTY_ATTRIBUTE_CURVE_PTR QStringLiteral("curve_ptr")

QwtChartSetDialog::QwtChartSetDialog(QWidget *parent,ChartWave_qwt* plot)
	: QWidget(parent)
	,m_plot(plot)
{
	createUI();
}

QwtChartSetDialog::~QwtChartSetDialog()
{

}

void QwtChartSetDialog::createUI()
{
	m_mainHorizontalLayout = new QHBoxLayout(this);
	m_mainHorizontalLayout->setSpacing(1);
	m_mainHorizontalLayout->setContentsMargins(1, 1, 1, 1);
	m_mainHorizontalLayout->setObjectName(QStringLiteral("mainHorizontalLayout"));
	m_mainHorizontalLayout->setContentsMargins(1, 1, 1, 1);

	m_property = new QtTreePropertyBrowser(this);
	m_mainHorizontalLayout->addWidget(m_property);

	m_propertyGroup = new QtGroupPropertyManager(m_property);
	m_variantManager = new QtVariantPropertyManager(m_property);
	m_variantFactory = new QtVariantEditorFactory(m_property);
	m_property->setFactoryForManager(m_variantManager,m_variantFactory);
	connect(m_variantManager,&QtVariantPropertyManager::valueChanged
		,this,&QwtChartSetDialog::onPropertyValueChanged);
	addPlotSet(m_plot);
	addCurveSet(m_plot);
}
void QwtChartSetDialog::addPlotSet(ChartWave_qwt* plot)
{
	//图表参数
	QtProperty *groupItem = m_propertyGroup->addProperty(QStringLiteral("图表参数"));
	m_property_id.rememberTheProperty("图表参数",groupItem);
	m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::String
		,groupItem,QStringLiteral("图表标题"),"图表标题",m_plot->title().text());
	m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::String
		,groupItem,QStringLiteral("脚标"),"脚标",m_plot->footer().text());
	m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::String
		,groupItem,QStringLiteral("X轴标题"),"X轴标题",m_plot->axisTitle(QwtPlot::xBottom).text());
	m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::String
		,groupItem,QStringLiteral("Y轴标题"),"Y轴标题",m_plot->axisTitle(QwtPlot::yLeft).text());
	m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::Color
		,groupItem,QStringLiteral("画布背景"),"画布背景",m_plot->canvasBackground().color());
    m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::Bool
        ,groupItem,QStringLiteral("缩放滚动条"),"缩放滚动条",m_plot->isEnableZoomerScroll());
	m_property->addProperty(groupItem);

}
void QwtChartSetDialog::addCurveSet(ChartWave_qwt* plot)
{
	QList<QwtPlotCurve*> curs = plot->getCurveList();
	for (auto ite = curs.begin();ite != curs.end();++ite)
	{
		addCurveSet(*ite);
	}
}

QString QwtChartSetDialog::penStyle2Chinese(const Qt::PenStyle penStyle)
{
	switch(penStyle){
	case Qt::SolidLine:
		return QStringLiteral("实线");
	case Qt::DashLine:
		return QStringLiteral("虚线");
	case Qt::DotLine:
		return QStringLiteral("点线");
	case Qt::DashDotLine:
		return QStringLiteral("点划线");
	case Qt::DashDotDotLine:
		return QStringLiteral("双点划线");
	}
	return QString();
}
Qt::PenStyle QwtChartSetDialog::Chinese2PenStyle(const QString str)
{
	if (str == QStringLiteral("实线"))
	{
		return Qt::SolidLine;
	}
	else if (str == QStringLiteral("点线"))
	{
		return Qt::DotLine;
	} 
	else if (str == QStringLiteral("点划线"))
	{
		return Qt::DashDotLine;
	}
	else if (str == QStringLiteral("双点划线"))
	{
		return Qt::DashDotDotLine;
	}
	return Qt::NoPen;
}

void QwtChartSetDialog::addCurveSet(QwtPlotCurve* curve)
{
	QString name = QStringLiteral("%1-图表参数").arg(curve->title().text());
	QtProperty *groupItem = m_propertyGroup->addProperty(name);
	QtVariantProperty *pro = nullptr;
	string groupId = QString::number((size_t)curve).toStdString();
	m_curIDs.append(groupId);
	m_property_id.rememberTheProperty(groupId,groupItem);
	pro = m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::String
		,groupItem,QStringLiteral("标题"),groupId+"标题",curve->title().text());
	m_property_curve[pro] = curve;
	const QPen pen = curve->pen();
	pro=  m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::Color
		,groupItem,QStringLiteral("颜色"),groupId+"颜色",pen.color());
	m_property_curve[pro] = curve;
	pro = m_property_id.addVariantPropertyInGroup(m_variantManager,QVariant::Int
		,groupItem,QStringLiteral("线宽"),groupId+"线宽",pen.width());
	pro->setAttribute(QStringLiteral("singleStep"), 1);
	m_property_curve[pro] = curve;
	pro = m_property_id.addVariantPropertyInGroup(m_variantManager,QtVariantPropertyManager::enumTypeId()
		,groupItem,QStringLiteral("线形"),groupId+"线形",pen.width());
	pro->setAttribute(QStringLiteral("enumNames"),QStringList()
		<<QStringLiteral("实线")
		<<QStringLiteral("虚线")
		<<QStringLiteral("点线")
		<<QStringLiteral("点划线")
		<<QStringLiteral("双点划线")
		);
	m_property_curve[pro] = curve;
	m_property->addProperty(groupItem);
}

QwtPlotCurve* QwtChartSetDialog::getCurvePtr(QtProperty * property)
{
	if (!property)
		return nullptr;
	QtVariantProperty *pro = dynamic_cast<QtVariantProperty*>(property);
	if (!pro)
		return nullptr;
	auto ite = m_property_curve.find(pro);
	if (ite == m_property_curve.end())
		return nullptr;
	return ite.value();
}

void QwtChartSetDialog::onPropertyValueChanged(QtProperty * property, const QVariant & value)
{
	string id = m_property_id.getPropertyID(property);
	QwtPlotCurve* cur = getCurvePtr(property);
	if (nullptr == cur)
	{
		if ("图表标题" == id)
		{
			m_plot->setTitle(value.toString());
		}
		else if ("脚标" == id )
		{
			m_plot->setFooter(value.toString());
		}
		else if ("X轴标题" == id)
		{
			m_plot->setAxisTitle(QwtPlot::xBottom,value.toString());
		}
		else if ("Y轴标题" == id)
		{
			m_plot->setAxisTitle(QwtPlot::yLeft,value.toString());
		}
		else if ("画布背景" == id)
		{
			m_plot->setCanvasBackground(QBrush(value.value<QColor>()));
		}
        else if("缩放滚动条" == id)
        {
            m_plot->enableZoomerScroll(value.toBool());
        }
	}
	else
	{
		string groupId = QString::number((size_t)cur).toStdString();		
		if ((groupId+"标题") == id)
		{
			cur->setTitle(value.toString());
			return;
		}
		QPen newPen(cur->pen());
		if ((groupId+"颜色") == id)
		{
			newPen.setColor(value.value<QColor>());
		}
		else if ((groupId+"线宽") == id)
		{
			newPen.setWidth(value.toInt());
		}
		else if ((groupId+"线形") == id)
		{
			newPen.setStyle(Qt::PenStyle(value.toInt()+1));
		}
		cur->setPen(newPen);
	}
}
