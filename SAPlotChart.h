#ifndef SAPLOTCHART_H
#define SAPLOTCHART_H

#include "chartwave_qwt.h"

#include <vector>
#include <QMap>
#include <QList>

#include <vector>
#include <QPointF>
#include <QTimer>
#include <QStandardItem>

#include <QwtPlotPropertySetDialog.h>

#include "QTDM.h"
///
/// \brief 用于记录常用的特征数值信息
///
#define STR_WDF_PointCounts QStringLiteral("数据点数")
#define STR_WDF_min_value QStringLiteral("最小值")
#define STR_WDF_max_value QStringLiteral("最大值")
#define STR_WDF_min_point QStringLiteral("最小点")
#define STR_WDF_max_point QStringLiteral("最大点")
#define STR_WDF_PeekPeekValue QStringLiteral("峰峰值")
#define STR_WDF_Mean QStringLiteral("均值")
#define STR_WDF_Sum QStringLiteral("和")
#define STR_WDF_Var QStringLiteral("方差")
#define STR_WDF_StdVar QStringLiteral("标准差")
#define STR_WDF_Skewness QStringLiteral("斜度")
#define STR_WDF_Kurtosis QStringLiteral("峭度/峰度")
#define STR_WDF_Top QStringLiteral("最大值序列")
#define STR_WDF_Low QStringLiteral("最小值序列")
#define STR_WDF_SharpPeakTop QStringLiteral("最大尖峰值")
#define STR_WDF_SharpPeakLow QStringLiteral("最小尖峰值")
#define STR_WDF_TEMP_ORDER QStringLiteral("order")
#ifndef TREE_ITEM_ROLE_POINT_VALUE
#define TREE_ITEM_ROLE_POINT_VALUE Qt::UserRole + 880
#endif

class ChartDataFeature{
public:
    ChartDataFeature(){
       // Max = Min = Mean = Sum = Var = StdVar = Skewness= Kurtosis =0;
    }
    ~ChartDataFeature(){}
public:

    //特征值
    QMap<QString,double> data_featureData;

    //特征点
    QMap<QString,QPointF> data_featurePoint;

    //特征点群
    QMap<QString,QVector<QPointF> > datas_featurePoints;

    typedef QMap<QString,double>::iterator ite_featureData;

    typedef QMap<QString,QPointF>::iterator ite_featurePoint;

    typedef QMap<QString,QVector<QPointF> >::iterator ite_featurePoints;

    typedef QVector<QPointF>::iterator ite_points;


    ite_featureData getIte_featureData_begin()
    {
        return data_featureData.begin();
    }
    ite_featureData getItefeatureData_end()
    {
        return data_featureData.end();
    }
    ite_featurePoint getIte_featurePoint_begin()
    {
        return data_featurePoint.begin();
    }
    ite_featurePoint getIte_featurePoint_end()
    {
        return data_featurePoint.end();
    }
    ite_featurePoints getIte_featurePoints_begin()
    {
        return datas_featurePoints.begin();
    }
    ite_featurePoints getIte_featurePoints_end()
    {
        return datas_featurePoints.end();
    }
};


class SAPlotChart : public ChartWave_qwt
{
    Q_OBJECT
public:
    SAPlotChart(QWidget* parent = nullptr);
    virtual ~SAPlotChart(){
    }
public:
    ///
    /// \brief setDatas 重写setDatas
    /// \param xyDatas
    /// \return
    ///
    QwtPlotCurve* addCurve(const QString& strName,std::vector<std::pair <double,double> >& xyDatas);
    QwtPlotCurve* addCurve(const QString& strName,std::vector<double>& xDatas,std::vector<double>& yDatas);
    QwtPlotCurve* addCurve(const QString& strName,const double *xData, const double *yData, int size);
    QwtPlotCurve* addCurve(const QString& strName,const QVector< double > &xData, const QVector< double > &yData);
    QwtPlotCurve* addCurve(const QString& strName,const QVector< QPointF > &datas);
    QwtPlotCurve* addVirtualCurve(const QString& strName,const QVector< QPointF > &datas);
    QwtPlotCurve* addVirtualCurve(const QString& strName,std::vector<double>& xDatas,std::vector<double>& yDatas);
    void addCurve(QwtPlotCurve* pC);

//    ModelTree_Property* getPropertyModel(){
//        return &m_property;
//    }
    ///
    /// \brief 获取波形特征值
    /// \param dataFeature 的引用
    /// \param pCurve 曲线指针
    /// \return 成功获取返回true
    ///	\note 此方法不会对特征值进行计算，只是获取已经经过计算的特征值.
    ///
    //bool getDataFeature(ChartDataFeature& dataFeature,const QString& strCurveName);
    bool getDataFeature(ChartDataFeature& dataFeature,QwtPlotCurve* pCurve);
    ///
    /// \brief 获取曲线的数据个数
    /// \param pCurve
    /// \return
    ///
    int getWaveDataCount(QwtPlotCurve* pCurve);
    void setTopLowCount(int nCount)
    {
        m_nTopLowCount = nCount;
    }
    int getTopLowCount()
    {
        return m_nTopLowCount;
    }

    ///
    /// \brief 计算尖峰值，就是获取尖峰值后进行排序
    /// \param cur
    /// \param count
    ///
    void calcSharpPeakPoint(QwtPlotCurve* cur, unsigned int count= 100);

    QVector<double> yDatas(QwtPlotCurve* cur);
    QVector<double> xDatas(QwtPlotCurve* cur);
    QVector<QPointF> datas(QwtPlotCurve* cur);
    ///
    /// \brief 计算曲线的特性
    /// \param pC 曲线的对应指针
    ///
    virtual void calcDataFeature(QwtPlotCurve* pC);
    ///
    /// \brief 计算数据的特性
    ///
    /// 这个静态函数将会计算最大最小均值等特征参数，并对数据点进行排序，排序后的数据点存在
    ///datas_featurePoints[STR_WDF_Low]中，由小到大进行排序，若需要提取其它的可以对datas_featurePoints[STR_WDF_Low]进行操作
    /// \param points 需要计算的数据
    /// \param out_chartData 计算得到的特征值
    ///
    static void calcDataFeature_s(const QVector<QPointF>& points,ChartDataFeature& out_chartData);
    ///
    /// \brief 计算曲线的特性
    /// \param pC 曲线的对应指针
    /// \param out_chartData 计算得到的特征值
    ///
    static void calcDataFeature_s(const QwtPlotCurve* pC,ChartDataFeature& out_chartData);
    ///
    /// \brief 计算曲线的特性,参数xInter用于指定需要计算的x范围
    ///
    /// 计算曲线的特性,参数xInter用于指定需要计算的x范围,也就是说它并不是把所有点进行计算，而是计算指定的范围
    /// 范围的指定可以参照类ChartWave_qwt的getPlottingRegionDatas方法源代码
    /// \param pC 曲线的对应指针
    /// \param xInter 曲线的对应指针
    /// \param out_chartData 计算得到的特征值
    ///
    static void calcDataFeature_s(QwtPlotCurve* pC,QwtInterval* xInter,ChartDataFeature& out_chartData);
    ///
    /// \brief 数据改变触发的信号
    ///
    //void DatasChang(const QString& strNameID);
    virtual void DatasChang(QwtPlotCurve* pC);
signals:
    void chartDataChang(QwtPlotCurve* pC);
protected:
    QList<QwtPlotCurve*> m_realPlotCurve; ///< 记录真正绘制的图像，对于一些标记线就不会进入这个list里，且是addCurve函数调用时才会加入这个list
private:
    QMap<QwtPlotCurve*,ChartDataFeature> m_DataFeature;///<数据的特质值
    int m_nTopLowCount;///< 记录排序的最大最小个数
};




#endif // SAPLOTCHART_H
