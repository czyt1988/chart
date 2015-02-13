#include "SAPlotChart.h"

#include <QDebug>
#include <vector>
#include "czyMath.h"
#include "czyMath_DSP.h"
#include "czyQArrayEx.h"
#include "czyQtApp.h"
#include "czyQtUI.h"
#include <QElapsedTimer>
#include <algorithm>
#include <memory>
#include <QToolBar>
#include <QComboBox>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QPen>
#include <QMessageBox>
#include "CurveSelectDialog.h"
static int static_pen_size_chang_data_count = 2048;


SAPlotChart::SAPlotChart(QWidget* parent):ChartWave_qwt(parent)
  ,m_nTopLowCount(100)
{
    setMinimumSize(160,120);//设置最小尺寸
}



QwtPlotCurve* SAPlotChart::addCurve(const QString& strName,std::vector<std::pair <double,double> >& xyDatas)
{
    QwtPlotCurve* pC = ChartWave_qwt::addCurve(xyDatas);
    pC->setTitle(strName);
    m_realPlotCurve.append(pC);
    calcDataFeature(pC);
    return pC;
}

QwtPlotCurve* SAPlotChart::addCurve(const QString& strName,std::vector<double>& xDatas,std::vector<double>& yDatas)
{
    QwtPlotCurve* pC =  ChartWave_qwt::addCurve(xDatas,yDatas);
    pC->setTitle(strName);
    m_realPlotCurve.append(pC);
    calcDataFeature(pC);
    return pC;
}

QwtPlotCurve* SAPlotChart::addCurve(const QString& strName,const double *xData, const double *yData, int size)
{
    QwtPlotCurve* pC =  ChartWave_qwt::addCurve(xData,yData,size);
    pC->setTitle(strName);
    m_realPlotCurve.append(pC);
    calcDataFeature(pC);
    return pC;
}

QwtPlotCurve* SAPlotChart::addCurve(const QString& strName,const QVector< double > &xData, const QVector< double > &yData){
    QwtPlotCurve* pC =  ChartWave_qwt::addCurve(xData,yData);
    pC->setTitle(strName);
    m_realPlotCurve.append(pC);
    calcDataFeature(pC);
    return pC;
}

void SAPlotChart::addCurve(QwtPlotCurve* pC)
{
    ChartWave_qwt::addCurve(pC);
    m_realPlotCurve.append(pC);
    calcDataFeature(pC);
}

QwtPlotCurve* SAPlotChart::addCurve(const QString& strName,const QVector< QPointF > &datas)
{
    QwtPlotCurve* pC =  ChartWave_qwt::addCurve(datas);
    pC->setTitle(strName);
    m_realPlotCurve.append(pC);
    calcDataFeature(pC);
    return pC;
}

QwtPlotCurve* SAPlotChart::addVirtualCurve(const QString& strName,const QVector< QPointF > &datas)
{
    QwtPlotCurve* pC =  ChartWave_qwt::addCurve(datas);
    pC->setTitle(strName);
    return pC;
}
QwtPlotCurve* SAPlotChart::addVirtualCurve(const QString& strName,std::vector<double>& xDatas,std::vector<double>& yDatas)
{
    QwtPlotCurve* pC =  ChartWave_qwt::addCurve(xDatas,yDatas);
    pC->setTitle(strName);
    return pC;
}

QVector<double> SAPlotChart::yDatas(QwtPlotCurve* cur)
{
    QVector<double> ys;
    auto datas = cur->data();
    size_t n = datas->size();
    ys.reserve(n);
    for(auto i=0;i<n;++i)
    {
        ys.append(datas->sample(i).y());
    }
    return ys;
}

QVector<double> SAPlotChart::xDatas(QwtPlotCurve* cur)
{
    QVector<double> xs;
    auto datas = cur->data();
    size_t n = datas->size();
    xs.reserve(n);
    for(auto i=0;i<n;++i)
    {
        xs.append(datas->sample(i).x());
    }
    return xs;
}
QVector<QPointF> SAPlotChart::datas(QwtPlotCurve* cur)
{
    QVector<QPointF> datas;
    auto pdatas = cur->data();
    size_t n = pdatas->size();
    datas.reserve(n);
    for(auto i=0;i<n;++i)
    {
        datas.append(pdatas->sample(i));
    }
    return datas;
}

void SAPlotChart::DatasChang(QwtPlotCurve* pC)
{
    if(nullptr == pC)
        return;
    calcDataFeature(pC);
}

// bool MdiChildChart::getDataFeature(ChartDataFeature& dataFeature
//                                             ,const QString& strCurveName)
// {
//     QwtPlotCurve* pC = getCurveByTitle(strCurveName);
//     if(nullptr == pC)
//         return false;
//     dataFeature = m_DataFeature[pC];
//     return true;
// }
bool SAPlotChart::getDataFeature(ChartDataFeature& dataFeature,QwtPlotCurve* pCurve)
{
    if(nullptr == pCurve)
        return false;
    dataFeature = m_DataFeature[pCurve];
    return true;
}

void SAPlotChart::calcDataFeature(QwtPlotCurve *pC)
{
    Q_CHECK_PTR(pC);

    ChartDataFeature dataFeature;
    calcDataFeature_s(pC,dataFeature);
    //由于calcDataFeature_s是把所有的数据都进行了排序，因此需要进行拆分

    size_t data_size = pC->data()->size();
    //在数据量小的时候进行复制操作，减少开销
    if (dataFeature.datas_featurePoints.contains(STR_WDF_TEMP_ORDER))
    {
        QVector<QPointF> pointe_Top,pointe_Low;
        QVector<QPointF>& orderPoints = dataFeature.datas_featurePoints[STR_WDF_TEMP_ORDER];
        QString top_name = STR_WDF_Top+QStringLiteral("Top%1").arg(m_nTopLowCount);
        QString low_name = STR_WDF_Low+QStringLiteral("Low%1").arg(m_nTopLowCount);
        dataFeature.datas_featurePoints[top_name] = pointe_Top;
        dataFeature.datas_featurePoints[low_name] = pointe_Low;

        dataFeature.data_featurePoint[STR_WDF_min_point] = orderPoints.front();
        dataFeature.data_featurePoint[STR_WDF_max_point] = orderPoints.back();
        size_t top_low_show_count = std::min(m_nTopLowCount,orderPoints.size());
        dataFeature.datas_featurePoints[top_name].resize(top_low_show_count);
        dataFeature.datas_featurePoints[low_name].resize(top_low_show_count);
        auto ite_top = dataFeature.datas_featurePoints[top_name].begin();
        auto ite_low = dataFeature.datas_featurePoints[low_name].begin();
        auto ite_sorted_low_points = orderPoints.begin();
        for(int i(0)
            ;i<top_low_show_count
            ;++i,++ite_top,++ite_low)
        {
            *ite_top = *(ite_sorted_low_points + data_size-1-i);
            *ite_low = *(ite_sorted_low_points + i);
        }
        //删除STR_WDF_Low
        dataFeature.datas_featurePoints.remove(STR_WDF_TEMP_ORDER);
    }

    m_DataFeature[pC] = dataFeature;
    QPen pen(pC->pen());
    pen.setColor(getRandLineColor());

    if (data_size<static_pen_size_chang_data_count)
    {
        pen.setWidth(2);
    }
    pC->setPen(pen);
    emit chartDataChang(pC);
}

void SAPlotChart::calcDataFeature_s(const QVector<QPointF>& points,ChartDataFeature& out_chartData)
{
    QVector<double> y;
    y.resize(points.size());
    std::transform(points.begin(),points.end(),y.begin()
                   ,[](const QPointF& f)->double
    {return f.y();});
    out_chartData.data_featureData[STR_WDF_PointCounts] = double(points.size());
    out_chartData.data_featureData[STR_WDF_min_value] = *(std::min_element(y.begin(),y.end()));
    out_chartData.data_featureData[STR_WDF_max_value] = *(std::max_element(y.begin(),y.end()));
    out_chartData.data_featureData[STR_WDF_PeekPeekValue] =
        out_chartData.data_featureData[STR_WDF_max_value] - out_chartData.data_featureData[STR_WDF_min_value];//峰峰值

    double Sum;
    double Mean;
    double Var;
    double StdVar;
    double Skewness;
    double Kurtosis;
    czy::Math::get_statistics(y.begin(),y.end(),Sum,Mean,Var
        ,StdVar,Skewness,Kurtosis);

    out_chartData.data_featureData[STR_WDF_Sum] = Sum;
    out_chartData.data_featureData[STR_WDF_Mean] = Mean;
    out_chartData.data_featureData[STR_WDF_Var] = Var;
    out_chartData.data_featureData[STR_WDF_StdVar] = StdVar;
    out_chartData.data_featureData[STR_WDF_Skewness] = Skewness;
    out_chartData.data_featureData[STR_WDF_Kurtosis] = Kurtosis;
    out_chartData.datas_featurePoints[STR_WDF_TEMP_ORDER] = points;

    std::sort(out_chartData.datas_featurePoints[STR_WDF_TEMP_ORDER].begin()
        ,out_chartData.datas_featurePoints[STR_WDF_TEMP_ORDER].end()
        ,ChartWave_qwt::cmpPointF_Y);

}

void SAPlotChart::calcDataFeature_s(const QwtPlotCurve* pC,ChartDataFeature& out_chartData)
{
    Q_CHECK_PTR(pC);

    QVector<QPointF> points;

    {
        auto datas = pC->data();
        size_t n = datas->size();
        points.reserve(n);
        for(auto i=0;i<n;++i)
        {
            points.append(QPointF(datas->sample(i).x(),datas->sample(i).y()));
        }
    }
    calcDataFeature_s(points,out_chartData);
}

void SAPlotChart::calcDataFeature_s(QwtPlotCurve* pC,QwtInterval* xInter,ChartDataFeature& out_chartData)
{
    Q_CHECK_PTR(pC);
    Q_CHECK_PTR(xInter);
    double min_x = xInter->minValue();
    double max_x = xInter->maxValue();
    QVector<QPointF> points;
    auto data_ptr = pC->data();
    size_t data_size = data_ptr->size();
    for (size_t i =0;i<data_size;++i)
    {
        if(data_ptr->sample(i).x()>=min_x
            &&
            data_ptr->sample(i).x()<= max_x)
        {
            points.push_back(data_ptr->sample(i));
        }
    }
    calcDataFeature_s(points,out_chartData);
}

int SAPlotChart::getWaveDataCount(QwtPlotCurve* pCurve)
{
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(pCurve->data());
    return int(datas->size());
}

void SAPlotChart::calcSharpPeakPoint(QwtPlotCurve* cur,unsigned int count)
{
    QVector<QPointF> sharpPoints_max,sharpPoints_min;
    sort_sharpPeak(sharpPoints_max,cur,true);
    if(sharpPoints_max.size() > count)
        sharpPoints_max.resize(count);
    m_DataFeature[cur].datas_featurePoints[STR_WDF_SharpPeakTop+QStringLiteral(" Top %1").arg(count)] = sharpPoints_max;

   sort_sharpPeak(sharpPoints_min,cur,false);
   if(sharpPoints_min.size() > count)
       sharpPoints_min.resize(count);
   m_DataFeature[cur].datas_featurePoints[STR_WDF_SharpPeakLow+QStringLiteral(" Low %1").arg(count)] = sharpPoints_min;

}
