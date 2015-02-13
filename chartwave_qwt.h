#ifndef CHARTWAVE_QWT_H
#define CHARTWAVE_QWT_H
#include <QMap>
#include <QPointF>

#include <qevent.h>
#include <qglobal.h>
#include <qscrollbar.h>
#include <QRectF>

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>
#include <QList>
#include <qwt_symbol.h>
#include <qwt_date.h>
#include <qwt_plot_legenditem.h>
#include <qwt_legend.h>
#include <qwt_date.h>

//class ScrollBar;
//class ScrollZoomer;

class ScrollData;

class ScrollBar: public QScrollBar
{
    Q_OBJECT

public:
    ScrollBar( QWidget *parent = NULL );
    ScrollBar( Qt::Orientation, QWidget *parent = NULL );
    ScrollBar( double minBase, double maxBase,
        Qt::Orientation o, QWidget *parent = NULL );

    void setInverted( bool );
    bool isInverted() const;

    double minBaseValue() const;
    double maxBaseValue() const;

    double minSliderValue() const;
    double maxSliderValue() const;

    int extent() const;

Q_SIGNALS:
    void sliderMoved( Qt::Orientation, double, double );
    void valueChanged( Qt::Orientation, double, double );

public Q_SLOTS:
    virtual void setBase( double min, double max );
    virtual void moveSlider( double min, double max );

protected:
    void sliderRange( int value, double &min, double &max ) const;
    int mapToTick( double ) const;
    double mapFromTick( int ) const;

private Q_SLOTS:
    void catchValueChanged( int value );
    void catchSliderMoved( int value );

private:
    void init();

    bool d_inverted;
    double d_minBase;
    double d_maxBase;
    int d_baseTicks;
};

class ScrollZoomer: public QwtPlotZoomer
{
    Q_OBJECT
public:
    enum ScrollBarPosition
    {
        AttachedToScale,
        OppositeToScale
    };

    ScrollZoomer( QWidget * );
    virtual ~ScrollZoomer();

    ScrollBar *horizontalScrollBar() const;
    ScrollBar *verticalScrollBar() const;

    void setHScrollBarMode( Qt::ScrollBarPolicy );
    void setVScrollBarMode( Qt::ScrollBarPolicy );

    Qt::ScrollBarPolicy vScrollBarMode () const;
    Qt::ScrollBarPolicy hScrollBarMode () const;

    void setHScrollBarPosition( ScrollBarPosition );
    void setVScrollBarPosition( ScrollBarPosition );

    ScrollBarPosition hScrollBarPosition() const;
    ScrollBarPosition vScrollBarPosition() const;

    QWidget* cornerWidget() const;
    virtual void setCornerWidget( QWidget * );

    virtual bool eventFilter( QObject *, QEvent * );

    virtual void rescale();
    bool isEnableScrollBar() const;
public slots:
    void on_enable_scrollBar(bool enable);
protected:
    virtual ScrollBar *scrollBar( Qt::Orientation );
    virtual void updateScrollBars();
    virtual void layoutScrollBars( const QRect & );
private Q_SLOTS:
    void scrollBarMoved( Qt::Orientation o, double min, double max );

private:
    bool needScrollBar( Qt::Orientation ) const;
    int oppositeAxis( int ) const;

    QWidget *d_cornerWidget;

    ScrollData *d_hScrollData;
    ScrollData *d_vScrollData;

    bool d_inZoom;
    bool d_alignCanvasToScales[ QwtPlot::axisCnt ];
    bool d_isEnable;///< 标定是否显示滚动条
};


class Zoomer_qwt: public ScrollZoomer
{
    Q_OBJECT
    const unsigned int c_rangeMax;
public:
    Zoomer_qwt( QWidget *canvas ):
        ScrollZoomer( canvas )
      ,c_rangeMax(1000)
    {
        setRubberBandPen( QColor( Qt::darkGreen ) );
 //       setRubberBandPen( QPen( Qt::red ) );
    }

    virtual void rescale()
    {
        QwtScaleWidget *scaleWidget = plot()->axisWidget( yAxis() );
        QwtScaleDraw *sd = scaleWidget->scaleDraw();

        double minExtent = 0.0;
        if ( zoomRectIndex() > 0 )
        {
            // When scrolling in vertical direction
            // the plot is jumping in horizontal direction
            // because of the different widths of the labels
            // So we better use a fixed extent.

            minExtent = sd->spacing() + sd->maxTickLength() + 1;
            minExtent += sd->labelSize(
                scaleWidget->font(), c_rangeMax ).width();
        }

        sd->setMinimumExtent( minExtent );

        ScrollZoomer::rescale();
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
///不出颜色？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
        return QwtText();
//
//         QString s;
//         s = QStringLiteral("(%1,%2)").arg(pos.x()).arg(pos.y());
//
//         QwtText text( s );
//         text.setColor( Qt::white );
//
//         QColor c = rubberBandPen().color();
//         text.setBorderPen( QPen( c ) );
//         text.setBorderRadius( 6 );
//         c.setAlpha( 170 );
//         text.setBackgroundBrush( c );
//
//         return text;
    }
};

///
/// \brief 箭头标志
///
class ArrowSymbol: public QwtSymbol
{
public:
    ///
    /// \brief ArrowSymbol
    /// \param clr 箭头颜色
    /// \param H 箭头高度（从下指上上）
    /// \param W 箭头总宽
    /// \param HL 箭头除去杆子的总高
    /// \param HC 箭头的内收缩高度
    ///
    ArrowSymbol(QColor clr = Qt::blue
            ,int H = 30
            ,int W = 8
            ,int HL = 6
            ,int HC = 12)
    {
        QPen pen(clr,0);
        pen.setJoinStyle( Qt::MiterJoin);

        setPen(pen);
        setBrush(clr);

        QPainterPath path;

		path.moveTo(0,-H);
        path.lineTo(0,-HL);
        path.lineTo(-(W/2),-HC);
        path.lineTo(0,0);
        path.lineTo((W/2),-HC);
        path.lineTo(0,-HL);

        setPath(path);
		setPinPointEnabled(true);
        setSize(W,H);
        setPinPoint(QPointF(0,0));//设置文字的坐标点，-6是偏移
    }
};

class LegendItem: public QwtPlotLegendItem
{
public:
	LegendItem()
	{
		setRenderHint( QwtPlotItem::RenderAntialiased );

		QColor color( Qt::white );

		setTextPen( color );

		setBorderPen( color );

		QColor c( Qt::gray );
		c.setAlpha( 200 );

		setBackgroundBrush( c );

	}
};

class CurveDataTracker: public QwtPlotPicker
{
public:
	CurveDataTracker( QWidget * );

protected:
	virtual QwtText trackerTextF( const QPointF & ) const;
	virtual QRect trackerRect( const QFont & ) const;

private:
	QString curveInfoAt( const QwtPlotCurve *, const QPointF & ) const;
	QLineF curveLineAt( const QwtPlotCurve *, double x ) const;
};
///
/// \brief 波形图控件 class
///
class ChartWave_qwt : public QwtPlot
{
    Q_OBJECT
public:
    ChartWave_qwt(QWidget *parent = nullptr);
	~ChartWave_qwt();
    //========================================================================================
    //画线和数据 操作
    //========================================================================================
    ///
    /// \brief 绘图
    /// \param xyDatas 波形图的xy数据
    /// \return
    ///
    QwtPlotCurve* addCurve(const QVector<QPointF>& xyDatas);
    QwtPlotCurve* addCurve(std::vector<std::pair <double,double> >& xyDatas);
    QwtPlotCurve* addCurve(std::vector<double>& xDatas,std::vector<double>& yDatas);
    QwtPlotCurve* addCurve(const double *xData, const double *yData, int size);
    QwtPlotCurve* addCurve(const QVector< double > &xData, const QVector< double > &yData);
    void addCurve(QwtPlotCurve* pC);
    QwtPlotMarker* addVLine(double val);
    QwtPlotMarker* addHLine(double val);
    //========================================================================================
    //网格 grid 操作
    //========================================================================================



    ///
    /// \brief getCureList 获取所有曲线
    /// \return
    ///
    QList<QwtPlotCurve*> getCurveList();
    ///
    /// \brief getMakerList 获取所有标记
    /// \return
    ///
    QList<QwtPlotMarker*> getMakerList();
    ///
    /// \brief 根据曲线名称获取曲线
    /// \param strName
    /// \return
    ///
    QwtPlotCurve* getCurveByTitle(const QString& strName);

    ///
    /// \brief 获取y轴数据
    /// \param ys
    /// \param nCur
    ///
    void getYDatas(QVector<double>& ys,int nCur);
    void getYDatas(QVector<double>& ys,const QString& strCurName);
    void getXDatas(QVector<double>& xs,int nCur);
    void getXDatas(QVector<double>& xs,const QString& strCurName);
    void getXYDatas(QVector<QPointF>& xys,int nCur);
    void getXYDatas(QVector<QPointF>& xys,const QString& strCurName);
    void getXYDatas(QVector<double>& xs,QVector<double>& ys,int nCur);
    void getXYDatas(QVector<double>& xs,QVector<double>& ys,const QString& strCurName);
    QRectF getPlottingRegionRang() const;
    QPoint getPlottingRegionDatas(QVector<QPointF>& out_xys,QwtPlotCurve* curve) const;
	QPoint getPlottingRegionDatas(std::vector<double>& out_xs
		,std::vector<double>& out_ys
        ,QwtPlotCurve* curve) const;
    size_t removeDataInRang(const QRectF& removeRang,QwtPlotCurve* curve);
//	QPoint getPlottingRegionDatasX_s(QwtPlotCurve* curve,QwtInterval xInter,QVector<QPointF>& out_xys);
	enum AxisDateScaleType{
	h_m,
	hh_mm,
	h_m_s,
	hh_mm_ss,
	yyyy_M_d,
	yyyy_M_d_h_m,
	yyyy_M_d_h_m_s,
	yyyy_MM_dd,
	yyyy_MM_dd_hh_mm,
	yyyy_MM_dd_hh_mm_ss
	};

	static QString axisDateScaleType2String(AxisDateScaleType type);

	void setDateAxis(AxisDateScaleType type,int axisID = xBottom,QwtDate::IntervalType intType = QwtDate::Second);
	void setDateAxis(QString type,int axisID = xBottom,QwtDate::IntervalType intType = QwtDate::Second);


    ///
    /// \brief 获取尖峰的点 - 所谓尖峰是指三点a,b,c b>a && b>c 就说明b是尖峰
    /// \param sharpPoints 尖峰值引用
    /// \param cur 需要获得数据的曲线
    /// \param getMax 获取的是上峰值
    ///
    static void getSharpPeakPoint(QVector<QPointF>& sharpPoints,QwtPlotCurve* cur,bool getMax = true);
    static void getSharpPeakPoint(QVector<QPointF>& sharpPoints,const QVector<QPointF>& Points,bool getMax = true);
    static int cmpPointF_Y(const QPointF& a,const QPointF& b)
    {
        return int( a.y() < b.y() );
    }
    ///
    /// \brief 尖峰排序
    /// \param sharpPoints
    /// \param cur
    /// \param getMax
    ///
    static void sort_sharpPeak(QVector<QPointF>& sharpPointsSorted,QwtPlotCurve* cur,bool getMax = true);
    static void sort_sharpPeak(QVector<QPointF>& sharpPointsSorted,const QVector<QPointF>& Points,bool getMax = true);
	static QColor getRandLineColor();
public slots:
	//功能性语句
    void enableZoomer(bool enable = true );


    void enableZoomerScroll(bool enable = true);


    void enablePicker(bool enable = true );
	void enableGrid(bool isShow = true);
	void enableGridX(bool enable = true);
	void enableGridY(bool enable = true);
	void enableGridXMin(bool enable = true);
	void enableGridYMin(bool enable = true);

	void enablePanner(bool enable = true);

	void enableLegend(bool enable = true);
	void enableLegendPanel(bool enable = true);

    void markPoint(QPointF pos,const QString& strLabel, QColor clr = Qt::black,int type = 0);

    void removeAllMarker();
	void showItem( const QVariant &itemInfo, bool on );

	void enableDataPicker(bool enable = true);


signals:
	void enableZoomerChanged(bool enable);
	void enablePickerChanged(bool enable);
	void enableGridChanged(bool enable);
	void enableGridXChanged(bool enable);
	void enableGridYChanged(bool enable);
	void enableGridXMinChanged(bool enable);
	void enableGridYMinChanged(bool enable);
	void enablePannerChanged(bool enable);
	void enableLegendChanged(bool enable);
	void enableLegendPanelChanged(bool enable);
	void enableDataPickerChanged(bool enable);
public:
    bool isEnableZoomer() const{return m_bEnableZoom;}
    bool isEnableZoomerScroll() const;
    ///
    /// \brief 是否允许十字光标
    /// \return
    ///
    bool isEnablePicker() const{return m_bEnableCrosserPicker;}
    bool isEnableGrid() const;
    bool isEnableGridX() const;
    bool isEnableGridY() const;
    bool isEnableGridXMin() const;
    bool isEnableGridYMin() const;
    bool isEnablePanner() const;
    bool isEnableLegend() const;
    bool isEnableLegendPanel() const;
    bool isEnableDataPicker() const;
protected:
    virtual void resizeEvent( QResizeEvent * );
private:
    QwtPlotGrid *m_grid;
    QwtPlotPicker *m_picker;
    QwtPlotZoomer *m_zoomer;
	QwtPlotPanner* m_panner;
	LegendItem* m_legend;
	QwtLegend* m_legendPanel;
	CurveDataTracker* m_dataPicker;
    bool m_bEnableZoom;
    bool m_bEnableCrosserPicker;
public:
	QwtPlotZoomer * zoomer(){return m_zoomer;}

public:
	///
	/// \brief 返回网格指针
	/// \return
	///
	QwtPlotGrid * grid(){
		return m_grid;
	}
private:
	///
	/// \brief 设置网格
	/// \param color 网格的颜色
	/// \param width 网格线条的宽度
	/// \param style 网格的样式
	/// \param bShowX 显示x坐标
	/// \param bShowY 显示y坐标
	/// \return
	///
	QwtPlotGrid* setupGrid(
		const QColor & 	color = Qt::gray
		,qreal 	width = 1.0
		,Qt::PenStyle style = Qt::DotLine);
	///
	/// \brief 移除网格
	///
	void deleteGrid();
    ///
    /// \brief 建立缩放模式
    ///
    void setupZoomer(bool isHaveScroll = true);
    ///
    /// \brief 建立一个内置的picker
    ///
    void setupPicker();
	///
	/// \brief 建立一个鼠标中间画布拖动
	///
	void setupPanner();
	void deletePanner();
	///
	/// \brief 建立一个图例r
	///
	void setupLegend();
	void setuplegendPanel();
	void deletelegendPanel();

	void setupDataPicker();
	void deleteDataPicker();
};


/*
 *

enum QwtPlotItem::RttiValues



Runtime type information.

RttiValues is used to cast plot items, without having to enable runtime type information of the compiler.



Enumerator



Rtti_PlotItem


Unspecific value, that can be used, when it doesn't matter.



Rtti_PlotGrid


For QwtPlotGrid.



Rtti_PlotScale


For QwtPlotScaleItem.



Rtti_PlotLegend


For QwtPlotLegendItem.



Rtti_PlotMarker


For QwtPlotMarker.



Rtti_PlotCurve


For QwtPlotCurve.



Rtti_PlotSpectroCurve


For QwtPlotSpectroCurve.



Rtti_PlotIntervalCurve


For QwtPlotIntervalCurve.



Rtti_PlotHistogram


For QwtPlotHistogram.



Rtti_PlotSpectrogram


For QwtPlotSpectrogram.



Rtti_PlotSVG


For QwtPlotSvgItem.



Rtti_PlotTradingCurve


For QwtPlotTradingCurve.



Rtti_PlotBarChart


For QwtPlotBarChart.



Rtti_PlotMultiBarChart


For QwtPlotMultiBarChart.



Rtti_PlotShape


For QwtPlotShapeItem.



Rtti_PlotTextLabel


For QwtPlotTextLabel.



Rtti_PlotZone


For QwtPlotZoneItem.



Rtti_PlotUserItem


Values >= Rtti_PlotUserItem are reserved for plot items not implemented in the Qwt library.


 */


#endif // CHARTWAVE_QWT_H
