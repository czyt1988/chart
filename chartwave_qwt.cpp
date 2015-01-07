#include "chartwave_qwt.h"
#include <qstyle.h>
#include <qstyleoption.h>

#include <qwt_interval.h>
#include <qwt_picker_machine.h>
#include <algorithm>
#include <qwt_legend_label.h>
#include <qwt_date_scale_draw.h>
//unsigned int ChartWave_qwt::staticValue_nAutoLineID = 0;//静态变量初始化


static const QColor static_const_qwt_line_color[14] = {
	QColor(240,52,121)//粉红
	,QColor(219,5,178)//紫色
	,QColor(65,152,175)//蓝色
	,QColor(169,155,189)//紫色
	,QColor(245,62,62)//橙色
	,QColor(54,80,234)//蓝色
	,QColor(36,36,36)//黑色
	,QColor(79,129,189)//蓝色
	,QColor(192,80,77)//红色
	,QColor(145,195,213)//蓝色
	,QColor(185,205,150)//绿色
	,QColor(36,183,103)//绿色
	,QColor(249,181,144)//橙色
	,QColor(137,165,78)//绿色
};
static const size_t static_const_qwt_line_color_count = 14;

QColor ChartWave_qwt::getRandLineColor()
{
	static size_t static_qwt_clrIndex = 0;
	++static_qwt_clrIndex;
	if (static_qwt_clrIndex>=static_const_qwt_line_color_count)
	{
		static_qwt_clrIndex = 0;
	}
	return static_const_qwt_line_color[static_qwt_clrIndex];
}

class ScrollData
{
public:
    ScrollData():
        scrollBar( NULL ),
        position( ScrollZoomer::OppositeToScale ),
        mode( Qt::ScrollBarAsNeeded )
    {
    }

    ~ScrollData()
    {
        delete scrollBar;
    }

    ScrollBar *scrollBar;
    ScrollZoomer::ScrollBarPosition position;
    Qt::ScrollBarPolicy mode;
};

class Picker_qwt: public QwtPlotPicker
{
public:
	Picker_qwt(int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QWidget *parent)
		:QwtPlotPicker(xAxis,yAxis,rubberBand,trackerMode,parent)
	{}
	Picker_qwt(QWidget *canvas):QwtPlotPicker(canvas)
	{
		setTrackerMode( QwtPlotPicker::AlwaysOn );//这是指定文字的显示，AlwaysOn值，光标不激活，也显示文字提示
		setRubberBand( QwtPlotPicker::CrossRubberBand );
		setStateMachine(  new QwtPickerTrackerMachine() );//QwtPickerTrackerMachine是不用鼠标激活
		//如果是new QwtPickerDragPointMachine()就是鼠标点击激活
	}
	~Picker_qwt(){}
	virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QString s("");
		const QwtScaleDraw* sd = plot()->axisScaleDraw(QwtPlot::xBottom);
		if(sd != nullptr)
		{
			const QwtDateScaleDraw* dsd = dynamic_cast<const QwtDateScaleDraw*>(sd);
			if(dsd != nullptr)
			{
				//说明坐标轴是时间轴
				s += QStringLiteral("(%1,").arg(dsd->label(pos.x()).text());
			}
			else
			{
				s += QStringLiteral("(%1,").arg(pos.x());
			}
		}
		sd = plot()->axisScaleDraw(QwtPlot::yLeft);
		if(sd != nullptr)
		{
			const QwtDateScaleDraw* dsd = dynamic_cast<const QwtDateScaleDraw*>(sd);
			if(dsd != nullptr)
			{
				//说明坐标轴是时间轴
				s += QStringLiteral("%1)").arg(dsd->label(pos.y()).text());
			}
			else
			{
				s += QStringLiteral("%2)").arg(pos.y());
			}
		}
		//axisScaleDraw
        

        QwtText text( s );
        text.setColor( Qt::white );

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 200 );
        text.setBackgroundBrush( c );

        return text;
    }
};


ScrollBar::ScrollBar( QWidget * parent ):
    QScrollBar( parent )
{
    init();
}

ScrollBar::ScrollBar( Qt::Orientation o,
        QWidget *parent ):
    QScrollBar( o, parent )
{
    init();
}

ScrollBar::ScrollBar( double minBase, double maxBase,
        Qt::Orientation o, QWidget *parent ):
    QScrollBar( o, parent )
{
    init();
    setBase( minBase, maxBase );
    moveSlider( minBase, maxBase );
}

void ScrollBar::init()
{
    d_inverted = orientation() == Qt::Vertical;
    d_baseTicks = 1000000;
    d_minBase = 0.0;
    d_maxBase = 1.0;
    moveSlider( d_minBase, d_maxBase );

    connect( this, SIGNAL( sliderMoved( int ) ), SLOT( catchSliderMoved( int ) ) );
    connect( this, SIGNAL( valueChanged( int ) ), SLOT( catchValueChanged( int ) ) );
}

void ScrollBar::setInverted( bool inverted )
{
    if ( d_inverted != inverted )
    {
        d_inverted = inverted;
        moveSlider( minSliderValue(), maxSliderValue() );
    }
}

bool ScrollBar::isInverted() const
{
    return d_inverted;
}

void ScrollBar::setBase( double min, double max )
{
    if ( min != d_minBase || max != d_maxBase )
    {
        d_minBase = min;
        d_maxBase = max;

        moveSlider( minSliderValue(), maxSliderValue() );
    }
}

void ScrollBar::moveSlider( double min, double max )
{
    const int sliderTicks = qRound( ( max - min ) /
        ( d_maxBase - d_minBase ) * d_baseTicks );

    // setRange initiates a valueChanged of the scrollbars
    // in some situations. So we block
    // and unblock the signals.

    blockSignals( true );

    setRange( sliderTicks / 2, d_baseTicks - sliderTicks / 2 );
    int steps = sliderTicks / 200;
    if ( steps <= 0 )
        steps = 1;

    setSingleStep( steps );
    setPageStep( sliderTicks );

    int tick = mapToTick( min + ( max - min ) / 2 );
    if ( isInverted() )
        tick = d_baseTicks - tick;

    setSliderPosition( tick );
    blockSignals( false );
}

double ScrollBar::minBaseValue() const
{
    return d_minBase;
}

double ScrollBar::maxBaseValue() const
{
    return d_maxBase;
}

void ScrollBar::sliderRange( int value, double &min, double &max ) const
{
    if ( isInverted() )
        value = d_baseTicks - value;

    const int visibleTicks = pageStep();

    min = mapFromTick( value - visibleTicks / 2 );
    max = mapFromTick( value + visibleTicks / 2 );
}

double ScrollBar::minSliderValue() const
{
    double min, dummy;
    sliderRange( value(), min, dummy );

    return min;
}

double ScrollBar::maxSliderValue() const
{
    double max, dummy;
    sliderRange( value(), dummy, max );

    return max;
}

int ScrollBar::mapToTick( double v ) const
{
    const double pos = ( v - d_minBase ) / ( d_maxBase - d_minBase ) * d_baseTicks;
    return static_cast<int>( pos );
}

double ScrollBar::mapFromTick( int tick ) const
{
    return d_minBase + ( d_maxBase - d_minBase ) * tick / d_baseTicks;
}

void ScrollBar::catchValueChanged( int value )
{
    double min, max;
    sliderRange( value, min, max );
    Q_EMIT valueChanged( orientation(), min, max );
}

void ScrollBar::catchSliderMoved( int value )
{
    double min, max;
    sliderRange( value, min, max );
    Q_EMIT sliderMoved( orientation(), min, max );
}

int ScrollBar::extent() const
{
    QStyleOptionSlider opt;
    opt.init( this );
    opt.subControls = QStyle::SC_None;
    opt.activeSubControls = QStyle::SC_None;
    opt.orientation = orientation();
    opt.minimum = minimum();
    opt.maximum = maximum();
    opt.sliderPosition = sliderPosition();
    opt.sliderValue = value();
    opt.singleStep = singleStep();
    opt.pageStep = pageStep();
    opt.upsideDown = invertedAppearance();
    if ( orientation() == Qt::Horizontal )
        opt.state |= QStyle::State_Horizontal;
    return style()->pixelMetric( QStyle::PM_ScrollBarExtent, &opt, this );
}

ScrollZoomer::ScrollZoomer( QWidget *canvas ):
    QwtPlotZoomer( canvas ),
    d_cornerWidget( NULL ),
    d_hScrollData( NULL ),
    d_vScrollData( NULL ),
    d_inZoom( false ),
    d_isEnable(true)
{
    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        d_alignCanvasToScales[ axis ] = false;

    if ( !canvas )
        return;

    d_hScrollData = new ScrollData;
    d_vScrollData = new ScrollData;
}

ScrollZoomer::~ScrollZoomer()
{
    delete d_cornerWidget;
    delete d_vScrollData;
    delete d_hScrollData;
}

void ScrollZoomer::rescale()
{
    QwtScaleWidget *xScale = plot()->axisWidget( xAxis() );
    QwtScaleWidget *yScale = plot()->axisWidget( yAxis() );

    if ( zoomRectIndex() <= 0 )
    {
        if ( d_inZoom )
        {
            xScale->setMinBorderDist( 0, 0 );
            yScale->setMinBorderDist( 0, 0 );

            QwtPlotLayout *layout = plot()->plotLayout();

            for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
                layout->setAlignCanvasToScale( axis, d_alignCanvasToScales );

            d_inZoom = false;
        }
    }
    else
    {
        if ( !d_inZoom )
        {
            /*
             We set a minimum border distance.
             Otherwise the canvas size changes when scrolling,
             between situations where the major ticks are at
             the canvas borders (requiring extra space for the label)
             and situations where all labels can be painted below/top
             or left/right of the canvas.
             */
            int start, end;

            xScale->getBorderDistHint( start, end );
            xScale->setMinBorderDist( start, end );

            yScale->getBorderDistHint( start, end );
            yScale->setMinBorderDist( start, end );

            QwtPlotLayout *layout = plot()->plotLayout();
            for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
            {
                d_alignCanvasToScales[axis] =
                    layout->alignCanvasToScale( axis );
            }

            layout->setAlignCanvasToScales( false );

            d_inZoom = true;
        }
    }

    QwtPlotZoomer::rescale();
    updateScrollBars();
}

bool ScrollZoomer::isEnableScrollBar() const
{
    return d_isEnable;
}

void ScrollZoomer::on_enable_scrollBar(bool enable)
{
    d_isEnable = enable;
    updateScrollBars();
}

ScrollBar *ScrollZoomer::scrollBar( Qt::Orientation orientation )
{
    ScrollBar *&sb = ( orientation == Qt::Vertical )
        ? d_vScrollData->scrollBar : d_hScrollData->scrollBar;

    if ( sb == NULL )
    {
        sb = new ScrollBar( orientation, canvas() );
        sb->hide();
        connect( sb,
            SIGNAL( valueChanged( Qt::Orientation, double, double ) ),
            SLOT( scrollBarMoved( Qt::Orientation, double, double ) ) );
    }
    return sb;
}

ScrollBar *ScrollZoomer::horizontalScrollBar() const
{
    return d_hScrollData->scrollBar;
}

ScrollBar *ScrollZoomer::verticalScrollBar() const
{
    return d_vScrollData->scrollBar;
}

void ScrollZoomer::setHScrollBarMode( Qt::ScrollBarPolicy mode )
{
    if ( hScrollBarMode() != mode )
    {
        d_hScrollData->mode = mode;
        updateScrollBars();
    }
}

void ScrollZoomer::setVScrollBarMode( Qt::ScrollBarPolicy mode )
{
    if ( vScrollBarMode() != mode )
    {
        d_vScrollData->mode = mode;
        updateScrollBars();
    }
}

Qt::ScrollBarPolicy ScrollZoomer::hScrollBarMode() const
{
    return d_hScrollData->mode;
}

Qt::ScrollBarPolicy ScrollZoomer::vScrollBarMode() const
{
    return d_vScrollData->mode;
}

void ScrollZoomer::setHScrollBarPosition( ScrollBarPosition pos )
{
    if ( d_hScrollData->position != pos )
    {
        d_hScrollData->position = pos;
        updateScrollBars();
    }
}

void ScrollZoomer::setVScrollBarPosition( ScrollBarPosition pos )
{
    if ( d_vScrollData->position != pos )
    {
        d_vScrollData->position = pos;
        updateScrollBars();
    }
}

ScrollZoomer::ScrollBarPosition ScrollZoomer::hScrollBarPosition() const
{
    return d_hScrollData->position;
}

ScrollZoomer::ScrollBarPosition ScrollZoomer::vScrollBarPosition() const
{
    return d_vScrollData->position;
}

void ScrollZoomer::setCornerWidget( QWidget *w )
{
    if ( w != d_cornerWidget )
    {
        if ( canvas() )
        {
            delete d_cornerWidget;
            d_cornerWidget = w;
            if ( d_cornerWidget->parent() != canvas() )
                d_cornerWidget->setParent( canvas() );

            updateScrollBars();
        }
    }
}

QWidget *ScrollZoomer::cornerWidget() const
{
    return d_cornerWidget;
}

bool ScrollZoomer::eventFilter( QObject *object, QEvent *event )
{
    if ( object == canvas() )
    {
        switch( event->type() )
        {
            case QEvent::Resize:
            {
                int left, top, right, bottom;
                canvas()->getContentsMargins( &left, &top, &right, &bottom );

                QRect rect;
                rect.setSize( static_cast<QResizeEvent *>( event )->size() );
                rect.adjust( left, top, -right, -bottom );

                layoutScrollBars( rect );
                break;
            }
            case QEvent::ChildRemoved:
            {
                const QObject *child =
                    static_cast<QChildEvent *>( event )->child();

                if ( child == d_cornerWidget )
                    d_cornerWidget = NULL;
                else if ( child == d_hScrollData->scrollBar )
                    d_hScrollData->scrollBar = NULL;
                else if ( child == d_vScrollData->scrollBar )
                    d_vScrollData->scrollBar = NULL;
                break;
            }
            default:
                break;
        }
    }
    return QwtPlotZoomer::eventFilter( object, event );
}

bool ScrollZoomer::needScrollBar( Qt::Orientation orientation ) const
{
    Qt::ScrollBarPolicy mode;
    double zoomMin, zoomMax, baseMin, baseMax;

    if ( orientation == Qt::Horizontal )
    {
        mode = d_hScrollData->mode;
        baseMin = zoomBase().left();
        baseMax = zoomBase().right();
        zoomMin = zoomRect().left();
        zoomMax = zoomRect().right();
    }
    else
    {
        mode = d_vScrollData->mode;
        baseMin = zoomBase().top();
        baseMax = zoomBase().bottom();
        zoomMin = zoomRect().top();
        zoomMax = zoomRect().bottom();
    }

    bool needed = false;
    switch( mode )
    {
        case Qt::ScrollBarAlwaysOn:
            needed = true;
            break;
        case Qt::ScrollBarAlwaysOff:
            needed = false;
            break;
        default:
        {
            if ( baseMin < zoomMin || baseMax > zoomMax )
                needed = true;
            break;
        }
    }
    return needed;
}

void ScrollZoomer::updateScrollBars()
{
    if ( !canvas() )
        return;

    const int xAxis = QwtPlotZoomer::xAxis();
    const int yAxis = QwtPlotZoomer::yAxis();

    int xScrollBarAxis = xAxis;
    if ( hScrollBarPosition() == OppositeToScale )
        xScrollBarAxis = oppositeAxis( xScrollBarAxis );

    int yScrollBarAxis = yAxis;
    if ( vScrollBarPosition() == OppositeToScale )
        yScrollBarAxis = oppositeAxis( yScrollBarAxis );


    QwtPlotLayout *layout = plot()->plotLayout();

    bool showHScrollBar = needScrollBar( Qt::Horizontal );
    if ( showHScrollBar )
    {
        ScrollBar *sb = scrollBar( Qt::Horizontal );
        sb->setPalette( plot()->palette() );
        sb->setInverted( !plot()->axisScaleDiv( xAxis ).isIncreasing() );
        sb->setBase( zoomBase().left(), zoomBase().right() );
        sb->moveSlider( zoomRect().left(), zoomRect().right() );

        if ( !sb->isVisibleTo( canvas() ) )
        {
            if(d_isEnable)
                sb->show();
            else
                sb->hide();
            layout->setCanvasMargin( layout->canvasMargin( xScrollBarAxis )
                + sb->extent(), xScrollBarAxis );
        }
    }
    else
    {
        if ( horizontalScrollBar() )
        {
            horizontalScrollBar()->hide();
            layout->setCanvasMargin( layout->canvasMargin( xScrollBarAxis )
                - horizontalScrollBar()->extent(), xScrollBarAxis );
        }
    }

    bool showVScrollBar = needScrollBar( Qt::Vertical );
    if ( showVScrollBar )
    {
        ScrollBar *sb = scrollBar( Qt::Vertical );
        sb->setPalette( plot()->palette() );
        sb->setInverted( plot()->axisScaleDiv( yAxis ).isIncreasing() );
        //如果sb->setInverted(! plot()->axisScaleDiv( yAxis ).isIncreasing() );那么向下拉滑动杆，视图向上滚
        sb->setBase( zoomBase().top(), zoomBase().bottom() );
        sb->moveSlider( zoomRect().top(), zoomRect().bottom() );

        if ( !sb->isVisibleTo( canvas() ) )
        {
            if(d_isEnable)
                sb->show();
            else
                sb->hide();
            layout->setCanvasMargin( layout->canvasMargin( yScrollBarAxis )
                + sb->extent(), yScrollBarAxis );
        }
    }
    else
    {
        if ( verticalScrollBar() )
        {
            verticalScrollBar()->hide();
            layout->setCanvasMargin( layout->canvasMargin( yScrollBarAxis )
                - verticalScrollBar()->extent(), yScrollBarAxis );
        }
    }

    if ( showHScrollBar && showVScrollBar )
    {
        if(d_isEnable)
        {
            if ( d_cornerWidget == NULL )
            {
                d_cornerWidget = new QWidget( canvas() );
                d_cornerWidget->setAutoFillBackground( true );
                d_cornerWidget->setPalette( plot()->palette() );
            }
            d_cornerWidget->show();
        }
        else
        {
            if ( d_cornerWidget )
                d_cornerWidget->hide();
        }
    }
    else
    {
        if ( d_cornerWidget )
            d_cornerWidget->hide();
    }

    layoutScrollBars( canvas()->contentsRect() );
    plot()->updateLayout();
}

void ScrollZoomer::layoutScrollBars( const QRect &rect )
{
    int hPos = xAxis();
    if ( hScrollBarPosition() == OppositeToScale )
        hPos = oppositeAxis( hPos );

    int vPos = yAxis();
    if ( vScrollBarPosition() == OppositeToScale )
        vPos = oppositeAxis( vPos );

    ScrollBar *hScrollBar = horizontalScrollBar();
    ScrollBar *vScrollBar = verticalScrollBar();

    const int hdim = hScrollBar ? hScrollBar->extent() : 0;
    const int vdim = vScrollBar ? vScrollBar->extent() : 0;

    if ( hScrollBar && hScrollBar->isVisible() )
    {
        int x = rect.x();
        int y = ( hPos == QwtPlot::xTop )
            ? rect.top() : rect.bottom() - hdim + 1;
        int w = rect.width();

        if ( vScrollBar && vScrollBar->isVisible() )
        {
            if ( vPos == QwtPlot::yLeft )
                x += vdim;
            w -= vdim;
        }

        hScrollBar->setGeometry( x, y, w, hdim );
    }
    if ( vScrollBar && vScrollBar->isVisible() )
    {
        int pos = yAxis();
        if ( vScrollBarPosition() == OppositeToScale )
            pos = oppositeAxis( pos );

        int x = ( vPos == QwtPlot::yLeft )
            ? rect.left() : rect.right() - vdim + 1;
        int y = rect.y();

        int h = rect.height();

        if ( hScrollBar && hScrollBar->isVisible() )
        {
            if ( hPos == QwtPlot::xTop )
                y += hdim;

            h -= hdim;
        }

        vScrollBar->setGeometry( x, y, vdim, h );
    }
    if ( hScrollBar && hScrollBar->isVisible() &&
        vScrollBar && vScrollBar->isVisible() )
    {
        if ( d_cornerWidget )
        {
            QRect cornerRect(
                vScrollBar->pos().x(), hScrollBar->pos().y(),
                vdim, hdim );
            d_cornerWidget->setGeometry( cornerRect );
        }
    }
}

void ScrollZoomer::scrollBarMoved(
    Qt::Orientation o, double min, double max )
{
    Q_UNUSED( max );

    if ( o == Qt::Horizontal )
        moveTo( QPointF( min, zoomRect().top() ) );
    else
        moveTo( QPointF( zoomRect().left(), min ) );

    Q_EMIT zoomed( zoomRect() );
}

int ScrollZoomer::oppositeAxis( int axis ) const
{
    switch( axis )
    {
        case QwtPlot::xBottom:
            return QwtPlot::xTop;
        case QwtPlot::xTop:
            return QwtPlot::xBottom;
        case QwtPlot::yLeft:
            return QwtPlot::yRight;
        case QwtPlot::yRight:
            return QwtPlot::yLeft;
        default:
            break;
    }

    return axis;
}


struct compareX
{
	inline bool operator()( const double x, const QPointF &pos ) const
	{
		return ( x < pos.x() );
	}
};

CurveDataTracker::CurveDataTracker( QWidget *canvas ):
QwtPlotPicker( canvas )
{
	setTrackerMode( QwtPlotPicker::ActiveOnly );
	setRubberBand( VLineRubberBand );

	setStateMachine( new QwtPickerDragPointMachine() );
}

QRect CurveDataTracker::trackerRect( const QFont &font ) const
{
	QRect r = QwtPlotPicker::trackerRect( font );

	// align r to the first curve
	
// 	const QwtPlotItemList curves = plot()->itemList( QwtPlotItem::Rtti_PlotCurve );
// 	if ( curves.size() > 0 )
// 	{
// 		QPointF pos = invTransform( trackerPosition() );
// 
// 		const QLineF line = curveLineAt(    
// 			static_cast<const QwtPlotCurve *>( curves[0] ), pos.x() );
// 		if ( !line.isNull() )
// 		{
// 			const double curveY = line.pointAt(
// 				( pos.x() - line.p1().x() ) / line.dx() ).y();
// 
// 			pos.setY( curveY );
// 			pos = transform( pos );
// 
// 			r.moveBottom( pos.y() );
// 		}
// 	}

	int heigth = r.height();
	r.moveTop(pickArea().boundingRect().top());
	return r;
}

QwtText CurveDataTracker::trackerTextF( const QPointF &pos ) const
{
	QwtText trackerText;

	trackerText.setColor( Qt::black );

	QColor c(200,200,200,100);
	trackerText.setBorderPen( QPen( c, 2 ) );
	trackerText.setBackgroundBrush( c );

	QString info;

	const QwtPlotItemList curves = 
		plot()->itemList( QwtPlotItem::Rtti_PlotCurve );

	for ( int i = 0; i < curves.size(); i++ )
	{
		const QString curveInfo = curveInfoAt( 
			static_cast<const QwtPlotCurve *>( curves[i] ), pos );

		if ( !curveInfo.isEmpty() )
		{
			if ( !info.isEmpty() )
				info += "<br>";

			info += curveInfo;
		}
	}

	trackerText.setText( info );
	return trackerText;
}

QString CurveDataTracker::curveInfoAt( 
	const QwtPlotCurve *curve, const QPointF &pos ) const
{
	const QLineF line = curveLineAt( curve, pos.x() );
	if ( line.isNull() )
		return QString::null;

	const double y = line.pointAt( 
		( pos.x() - line.p1().x() ) / line.dx() ).y();
	
	QString info( "<font color=""%1"">%2</font>" );
	return info.arg( curve->pen().color().name() ).arg( y );
}

QLineF CurveDataTracker::curveLineAt( 
	const QwtPlotCurve *curve, double x ) const
{
	QLineF line;

	if ( curve->dataSize() >= 2 )
	{
		const QRectF br = curve->boundingRect();
		if ( br.isValid() && x >= br.left() && x <= br.right() )
		{
			int index = qwtUpperSampleIndex<QPointF>( 
				*curve->data(), x, compareX() );

			if ( index == -1 && 
				x == curve->sample( curve->dataSize() - 1 ).x() )
			{
				// the last sample is excluded from qwtUpperSampleIndex
				index = curve->dataSize() - 1;
			}

			if ( index > 0 )
			{
				line.setP1( curve->sample( index - 1 ) );
				line.setP2( curve->sample( index ) );
			}
		}
	}

	return line;
}

ChartWave_qwt::ChartWave_qwt(QWidget *parent):QwtPlot(parent)
  ,m_grid(nullptr)
  ,m_zoomer(nullptr)
  ,m_picker(nullptr)
  ,m_panner(nullptr)
  ,m_legend(nullptr)
  ,m_legendPanel(nullptr)
  ,m_dataPicker(nullptr)
  ,m_bEnableZoom(false)
  ,m_bEnableCrosserPicker(false)
{
    setAutoReplot( false );
    setAutoFillBackground(true);

    QwtPlotCanvas* pCanvas = new QwtPlotCanvas();
    //pCanvas->setLineWidth( 0 );
	pCanvas->setAutoFillBackground(false);
    pCanvas->setFrameStyle( QFrame::NoFrame );
    //pCanvas->setBorderRadius( 0 );//设置圆角为0

    QPalette canvasPalette( Qt::white );
    //canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    pCanvas->setPalette( canvasPalette );
    setCanvas( pCanvas );
	setPalette( Qt::white );
    setAutoReplot( true );
}

ChartWave_qwt::~ChartWave_qwt()
{

}

void ChartWave_qwt::resizeEvent( QResizeEvent *event )
{
    QwtPlot::resizeEvent( event );
	
    // Qt 4.7.1: QGradient::StretchToDeviceMode is buggy on X11
    //updateGradient();
}

bool ChartWave_qwt::isEnableZoomerScroll() const
{
    Zoomer_qwt* zm = qobject_cast<Zoomer_qwt*>(m_zoomer);
    if(zm)
    {
        return zm->isEnableScrollBar();
    }
    return false;
}
//========================================================================================
//网格 grid 操作
//========================================================================================
QwtPlotGrid* ChartWave_qwt::setupGrid(
        const QColor & 	color
        ,qreal 	width
        ,Qt::PenStyle style)
{
    bool bShouldAttachAgain(false);
    if(nullptr == m_grid){
        m_grid = new QwtPlotGrid;
        bShouldAttachAgain = true;
    }
    //大刻度显示网格- 所谓大刻度是值刻度上带数值的
    m_grid->setMajorPen( color, width, style );
	m_grid->setMinorPen( color, 0 , Qt::DotLine );//小刻度的样式
    if(bShouldAttachAgain){
        m_grid->attach( this );
    }
    return m_grid;
}

void ChartWave_qwt::deleteGrid(){
    if(nullptr == m_grid){
        return;
    }
    m_grid->detach();
    delete m_grid;
    m_grid = nullptr;
    replot();//刷新，否则不显示
}

void ChartWave_qwt::enableGrid(bool isShow)
{
	if (isShow)
	{
		if(nullptr == m_grid){
			setupGrid();
		}
		m_grid->enableX(true);
		m_grid->enableY(true);
		m_grid->show();
		emit enableGridXChanged(isShow);
		emit enableGridYChanged(isShow);
		emit enableGridChanged(isShow);
		return;
	}
	else
	{
		if(nullptr == m_grid){
			return;
		}
		m_grid->hide();
	}
    replot();
	emit enableGridChanged(isShow);
}

void ChartWave_qwt::enableGridX(bool enable)
{
	if (nullptr == m_grid)
		return;
	m_grid->enableX(enable);
	emit enableGridXChanged(enable);
	if (!enable)
	{
		emit enableGridXMinChanged(false);
	}
	//m_grid->show();//刷新
}

void ChartWave_qwt::enableGridY(bool enable)
{
	if (nullptr == m_grid)
		return;
	m_grid->enableY(enable);
	emit enableGridYChanged(enable);
	if (!enable)
	{
		emit enableGridYMinChanged(false);
	}
}

void ChartWave_qwt::enableGridXMin(bool enable)
{
	if (nullptr == m_grid)
		return;
	m_grid->enableXMin(enable);
	emit enableGridXMinChanged(enable);
}
void ChartWave_qwt::enableGridYMin(bool enable)
{
	if (nullptr == m_grid)
		return;
	m_grid->enableYMin(enable);
	emit enableGridYMinChanged(enable);
}

//========================================================================================
//画线和数据 操作
//========================================================================================

QwtPlotCurve* ChartWave_qwt::addCurve(const QVector<QPointF>& xyDatas)
{
    QwtPlotCurve* pCurve = nullptr;
    pCurve = new QwtPlotCurve;
    pCurve->setStyle(QwtPlotCurve::Lines);
    pCurve->setSamples(xyDatas);
    pCurve->attach(this);
    return pCurve;
}

QwtPlotCurve* ChartWave_qwt::addCurve(std::vector<std::pair<double,double>>& xyDatas)
{
    QwtPlotCurve* pCurve = nullptr;
    QVector<double> x,y;
    x.reserve(xyDatas.size());
    y.reserve(xyDatas.size());
    std::vector<std::pair<double,double>>::iterator ite;
    for(ite = xyDatas.begin();ite != xyDatas.end();++ite){
        x.push_back(ite->first);
        y.push_back(ite->second);
    }
    pCurve = new QwtPlotCurve;
  //  pCurve->setRenderHint( QwtPlotItem::RenderAntialiased );
    pCurve->setStyle(QwtPlotCurve::Lines);
    pCurve->setSamples(x,y);
    pCurve->attach(this);

    return pCurve;
}
QwtPlotCurve* ChartWave_qwt::addCurve(std::vector<double>& xDatas,std::vector<double>& yDatas)
{
    QwtPlotCurve* pCurve = nullptr;
    pCurve = new QwtPlotCurve;
    pCurve->setStyle(QwtPlotCurve::Lines);
    pCurve->setSamples(&xDatas[0],&yDatas[0],xDatas.size() <= yDatas.size() ? xDatas.size() : yDatas.size());
    pCurve->attach(this);
    return pCurve;
}

QwtPlotCurve* ChartWave_qwt::addCurve(const double *xData, const double *yData, int size)
{
    QwtPlotCurve* pCurve = nullptr;
    pCurve = new QwtPlotCurve;
    pCurve->setStyle(QwtPlotCurve::Lines);
    pCurve->setSamples(xData,yData,size);
    pCurve->attach(this);
    return pCurve;
}

QwtPlotCurve* ChartWave_qwt::addCurve(const QVector< double > &xData
                                      , const QVector< double > &yData)
{
    QwtPlotCurve* pCurve = nullptr;
    pCurve = new QwtPlotCurve;
    pCurve->setStyle(QwtPlotCurve::Lines);
    pCurve->setSamples(xData,yData);
    pCurve->attach(this);
    return pCurve;
}

void ChartWave_qwt::addCurve(QwtPlotCurve* pC)
{
    pC->attach(this);
}
///
/// \brief 添加一条竖直线
/// \return
///
QwtPlotMarker* ChartWave_qwt::addVLine(double val)
{
    QwtPlotMarker *marker = new QwtPlotMarker();
    marker->setXValue(val);
    marker->setLineStyle( QwtPlotMarker::VLine );
    marker->setItemAttribute( QwtPlotItem::Legend, true );
    marker->attach( this );
    return marker;
}
QwtPlotMarker* ChartWave_qwt::addHLine(double val)
{
    QwtPlotMarker *marker = new QwtPlotMarker();
    marker->setYValue(val);
    marker->setLineStyle( QwtPlotMarker::HLine );
    marker->setItemAttribute( QwtPlotItem::Legend, true );
    marker->attach( this );
    return marker;
}

///
/// \brief 在图片上标记点
/// \param pos 点的位置
/// \param strLabel 说明
/// \param type 样式，默认为0，就是箭头
///
void ChartWave_qwt::markPoint(QPointF pos, const QString& strLabel, QColor clr, int type)
{

    QwtPlotMarker* arrMark = new QwtPlotMarker(QStringLiteral("%1,%2 - arr")
		.arg(pos.x())
		.arg(pos.y())
		);
    arrMark->setRenderHint( QwtPlotItem::RenderAntialiased, true );
	//arrMark->setItemAttribute( QwtPlotItem::Legend, true );//AutoScale 加了这个会在legend里显示
	ArrowSymbol* arr = new ArrowSymbol(clr);//Symbol在QwtPlotMarker内部不会删除，除非再setSymbol(nullptr)

	arrMark->setSymbol( arr );
    arrMark->setValue(pos);
    arrMark->setLabel(strLabel);
    arrMark->setLabelAlignment(Qt::AlignTop|Qt::AlignHCenter);
	arrMark->setSpacing(15);//设置文字和mark的间隔
    arrMark->attach( this );


        ///未完成，注意
 }
///
/// \brief 擦除所有的标记
///
void ChartWave_qwt::removeAllMarker()
{
    detachItems(QwtPlotItem::Rtti_PlotMarker);
}


void ChartWave_qwt::setupPicker()
{
    if(nullptr == m_picker)
    {
        //m_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        //    QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,canvas() );
// 		m_picker = new Picker_qwt( QwtPlot::xBottom, QwtPlot::yLeft,
//             QwtPlotPicker::CrossRubberBand,QwtPlotPicker::ActiveOnly /*QwtPicker::AlwaysOn*/,canvas() );
		m_picker = new Picker_qwt( this->canvas() );
        m_picker->setRubberBandPen(  QPen( QColor(186,85,211) ) );//QPen( "MediumOrchid" )

    }

}

void ChartWave_qwt::enablePicker(bool enable)
{
    if(nullptr == m_picker)
        setupPicker();
	m_picker->setEnabled( enable );
    if(nullptr != m_zoomer)
    {
        if(enable && isEnableZoom())
            m_zoomer->setTrackerMode( QwtPicker::AlwaysOff );
        if(!enable && isEnableZoom())
            m_zoomer->setTrackerMode( QwtPicker::AlwaysOn );
    }
    m_bEnableCrosserPicker = enable;
	emit enablePickerChanged(enable);
}

void ChartWave_qwt::setupZoomer(bool isHaveScroll)
{
    if(nullptr == m_zoomer )
    {
        if(isHaveScroll)
        {//带滚动条的缩放
            m_zoomer = new Zoomer_qwt(canvas());//Zoomer_qwt( QwtPlot::xBottom, QwtPlot::yLeft,canvas() );
        }
        else
        {//不带滚动条的缩放
            m_zoomer = new QwtPlotZoomer(canvas());
        }
        m_zoomer->setRubberBand( QwtPicker::RectRubberBand );

        m_zoomer->setTrackerMode( QwtPicker::ActiveOnly );
        m_zoomer->setTrackerPen( QColor( Qt::black ) );

        m_zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
                Qt::RightButton, Qt::ControlModifier );
        m_zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
                Qt::RightButton );
    }
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier( canvas() );
    magnifier->setMouseButton( Qt::NoButton );

}
void ChartWave_qwt::enablePanner(bool enable)
{
	if (enable)
	{
		if (nullptr == m_panner)
		{
			setupPanner();
		}
		m_panner->setEnabled(enable);
	}
	else
	{
		if (nullptr != m_panner)
		{
			m_panner->setEnabled(enable);
		}
	}
	emit enablePannerChanged(enable);
}
///
/// \brief 建立一个内置的Panner(拖动)，默认使用鼠标中键
///
void ChartWave_qwt::setupPanner()
{
	//设置拖动
	if (nullptr == m_panner)
	{
		m_panner = new QwtPlotPanner( canvas() );
		m_panner->setMouseButton( Qt::MidButton );
	}
}
void ChartWave_qwt::deletePanner()
{
	if (nullptr != m_panner)
	{
		m_panner->setParent(nullptr);//断开和canvas()的父子连接
		delete m_panner;
		m_panner = nullptr;
	}
}

void ChartWave_qwt::enableZoomer(bool enable)
{
    if(nullptr == m_zoomer)
        setupZoomer();
    m_zoomer->setEnabled(enable);
    if(enable && isEnableCrosserPicker())//如果十指光标激活了，就关闭坐标提示
        m_zoomer->setTrackerMode( QwtPicker::AlwaysOff );
    if(enable && !isEnableCrosserPicker())
        m_zoomer->setTrackerMode( QwtPicker::AlwaysOn );
    m_zoomer->zoom( 0 );
    m_bEnableZoom = enable;
    emit enableZoomerChanged(enable);
}
///
/// \brief 设置是否显示滚动条
/// \param enable
///
void ChartWave_qwt::enableZoomerScroll(bool enable)
{
    Zoomer_qwt* zm = qobject_cast<Zoomer_qwt*>(m_zoomer);
    if(zm)
    {
        zm->on_enable_scrollBar(enable);
    }
}

void ChartWave_qwt::setupLegend()
{
	if (nullptr == m_legend)
	{
		m_legend = new LegendItem();
		m_legend->attach( this );
	}
}

void ChartWave_qwt::enableLegend(bool enable)
{
	if (enable)
	{
		if (m_legend)
		{
			m_legend->setVisible(true);
		}
		else
		{
			setupLegend();
		}
	}
	else
	{
		if (m_legend)
		{
			m_legend->setVisible(false);
// 			m_legend->detach();
// 			delete m_legend;
// 			m_legend = nullptr;
		}
	}
	emit enableLegendChanged(enable);
}

void ChartWave_qwt::enableLegendPanel(bool enable )
{
	if (enable)
	{
		setuplegendPanel();	
	}
	else
	{
		deletelegendPanel();
	}
	emit enableLegendPanelChanged(enable);
}
void ChartWave_qwt::setuplegendPanel()
{
	if (m_legendPanel)
	{
		return;
	}
	m_legendPanel = new QwtLegend;
	m_legendPanel->setDefaultItemMode( QwtLegendData::Checkable );
	insertLegend( m_legendPanel, QwtPlot::RightLegend );
//	connect( m_legendPanel, &QwtLegend::checked,&ChartWave_qwt::showItem);
	connect( m_legendPanel, SIGNAL( checked( const QVariant &, bool, int ) ),
		SLOT( showItem( const QVariant &, bool ) ) );

	QwtPlotItemList items = itemList( QwtPlotItem::Rtti_PlotCurve );
	for ( int i = 0; i < items.size(); i++ )
	{
		const QVariant itemInfo = itemToInfo( items[i] );
		QwtLegendLabel *legendLabel =
			qobject_cast<QwtLegendLabel *>( m_legendPanel->legendWidget( itemInfo ) );
		if ( legendLabel )
			legendLabel->setChecked( items[i]->isVisible() );
	}
}
void ChartWave_qwt::deletelegendPanel()
{
	insertLegend( nullptr);//内部会销毁
	m_legendPanel = nullptr;
}

void ChartWave_qwt::setupDataPicker()
{
	if (nullptr == m_dataPicker)
	{
		m_dataPicker = new CurveDataTracker( this->canvas() );
		m_dataPicker->setStateMachine( new QwtPickerTrackerMachine() );
		m_dataPicker->setRubberBandPen( QPen( "MediumOrchid" ) );
	}
}
void ChartWave_qwt::deleteDataPicker()
{
	if (nullptr != m_dataPicker)
	{
		m_dataPicker->setParent(nullptr);//断开和canvas()的父子连接
		delete m_dataPicker;
		m_dataPicker = nullptr;
	}
}
void ChartWave_qwt::enableDataPicker(bool enable)
{
	if (enable)
	{
		setupDataPicker();	
	}
	else
	{
		deleteDataPicker();
	}
	emit enableDataPickerChanged(enable);
}
void ChartWave_qwt::showItem( const QVariant &itemInfo, bool on )
{
	QwtPlotItem *plotItem = infoToItem( itemInfo );
	if ( plotItem )
		plotItem->setVisible( on );
}

QList<QwtPlotCurve*> ChartWave_qwt::getCurveList()
{
    QList<QwtPlotCurve*> curves;
    QwtPlotItemList items = itemList(QwtPlotItem::Rtti_PlotCurve);
    for(int i(0);i<items.size();++i){
        curves.append(static_cast<QwtPlotCurve*>( items[i] ));
    }
    return curves;
}
QList<QwtPlotMarker*> ChartWave_qwt::getMakerList()
{
    QList<QwtPlotMarker*> list;
    QwtPlotItemList items = itemList(QwtPlotItem::Rtti_PlotMarker);
    for(int i(0);i<items.size();++i){
        list.append(static_cast<QwtPlotMarker*>( items[i] ));
    }
    return list;
}

QwtPlotCurve* ChartWave_qwt::getCurveByTitle(const QString& strName)
{
    QList<QwtPlotCurve*> curs = getCurveList();
    QwtPlotCurve* cur(nullptr);
    for(int i(0);i<curs.size();++i){
        cur = curs[i];
        if(cur)
        {
            if(strName == cur->title().text())
                return cur;
        }
    }
    return nullptr;
}

void ChartWave_qwt::getYDatas(QVector<double>& ys,int nCur)
{
    QwtPlotCurve* cur = getCurveList().at(nCur);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    ys = datas->yData();
}

void ChartWave_qwt::getYDatas(QVector<double>& ys,const QString& strCurName)
{
    QwtPlotCurve* cur = getCurveByTitle(strCurName);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    ys = datas->yData();
}

void ChartWave_qwt::getXDatas(QVector<double>& xs,int nCur)
{
    QwtPlotCurve* cur = getCurveList().at(nCur);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    xs = datas->xData();
}

void ChartWave_qwt::getXDatas(QVector<double>& xs,const QString& strCurName)
{
    QwtPlotCurve* cur = getCurveByTitle(strCurName);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    xs = datas->xData();
}

void ChartWave_qwt::getXYDatas(QVector<QPointF>& xys,int nCur)
{
    QwtPlotCurve* cur = getCurveList().at(nCur);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    xys.reserve(datas->size());
    for(unsigned int i=0;i< datas->size();++i)
    {
        xys.append(datas->sample(i));
    }
}

void ChartWave_qwt::getXYDatas(QVector<QPointF>& xys,const QString& strCurName)
{
    QwtPlotCurve* cur = getCurveByTitle(strCurName);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    xys.reserve(datas->size());
    for(unsigned int i=0;i< datas->size();++i)
    {
        xys.append(datas->sample(i));
    }
}

void ChartWave_qwt::getXYDatas(QVector<double>& xs,QVector<double>& ys,int nCur)
{
    QwtPlotCurve* cur = getCurveList().at(nCur);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    xs = datas->xData();
    ys = datas->yData();
}

void ChartWave_qwt::getXYDatas(QVector<double>& xs,QVector<double>& ys,const QString& strCurName)
{
    QwtPlotCurve* cur = getCurveByTitle(strCurName);
    if(nullptr == cur){
        return;
    }
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas){
        return;
    }
    xs = datas->xData();
    ys = datas->yData();
}

void ChartWave_qwt::getSharpPeakPoint(QVector<QPointF>& sharpPoints,QwtPlotCurve* cur,bool getMax)
{
    QwtPointArrayData* datas = static_cast<QwtPointArrayData*>(cur->data());
    if(nullptr == datas)
        return;
    QVector<QPointF> points;
    points.reserve(datas->size());
    for(unsigned int i(0);i<datas->size();++i)
    {
        points.append(datas->sample(i));
    }
    getSharpPeakPoint(sharpPoints,points,getMax);
}

void ChartWave_qwt::getSharpPeakPoint(QVector<QPointF>& sharpPoints,const QVector<QPointF>& Points,bool getMax)
{
    sharpPoints.clear();
    sharpPoints.reserve(int(Points.size()/2));
    int maxLoop = Points.size()-1;

    if(getMax)
    {
        for(int i=1;i<maxLoop;++i)
        {
            if((Points[i].y() > Points[i-1].y()) && (Points[i].y() > Points[i+1].y()))
            {
                sharpPoints.append(Points[i]);
            }
        }
    }
    else
    {
        for(int i=1;i<maxLoop;++i)
        {
            if((Points[i].y() < Points[i-1].y()) && (Points[i].y() < Points[i+1].y()))
            {
                sharpPoints.append(Points[i]);
            }
        }
    }
}


void ChartWave_qwt::sort_sharpPeak(QVector<QPointF>& sharpPointsSorted,QwtPlotCurve* cur,bool getMax )
{
    getSharpPeakPoint(sharpPointsSorted,cur,getMax);
    std::sort(sharpPointsSorted.begin(),sharpPointsSorted.end(),cmpPointF_Y);
    std::reverse(sharpPointsSorted.begin(),sharpPointsSorted.end());//最大值需要进行一次翻转
}

void ChartWave_qwt::sort_sharpPeak(QVector<QPointF>& sharpPointsSorted,const QVector<QPointF>& Points,bool getMax )
{
    getSharpPeakPoint(sharpPointsSorted,Points,getMax);
    std::sort(sharpPointsSorted.begin(),sharpPointsSorted.end(),cmpPointF_Y);
}
///
/// \brief 获取当前显示区域的数据
/// \param out_xys
/// \param curve
/// \return 返回区域的索引x为第一个索引，y为第二个索引
///
QPoint ChartWave_qwt::getPlottingRegionDatas(QVector<QPointF>& out_xys,QwtPlotCurve* curve) const
{
	if (!curve)
		return QPoint(0,0);
	QPoint boundary(0,0);
    QwtPlot::Axis xaxis = QwtPlot::xBottom;
    if(!axisEnabled(QwtPlot::xBottom))
        xaxis = QwtPlot::xTop;
    QwtInterval xInter = axisInterval(xaxis);
	double min = xInter.minValue();
	double max = xInter.maxValue();

	auto pdatas = curve->data();
	size_t n = pdatas->size();
	out_xys.reserve(n);
	bool firstIn(true);
	for(auto i=0;i<n;++i)
	{
		if(pdatas->sample(i).x()>=min 
			&&
			pdatas->sample(i).x()<= max)
		{
			out_xys.push_back(pdatas->sample(i));
			if (firstIn)
			{
				boundary.rx() = i;
				firstIn = false;
			}
			boundary.ry() = i;
		}
	}
	return boundary;
}

QPoint ChartWave_qwt::getPlottingRegionDatas(std::vector<double>& out_xs
	,std::vector<double>& out_ys
    ,QwtPlotCurve* curve) const
{
	if (!curve)
		return QPoint(0,0);
	QPoint boundary(0,0);
    QwtPlot::Axis xaxis = QwtPlot::xBottom;
    if(!axisEnabled(QwtPlot::xBottom))
        xaxis = QwtPlot::xTop;
    QwtInterval xInter = axisInterval(xaxis);
	double min = xInter.minValue();
	double max = xInter.maxValue();

	auto pdatas = curve->data();
	size_t n = pdatas->size();
	out_xs.reserve(n);
	out_ys.reserve(n);
	bool firstIn(true);
    for(size_t i=0;i<n;++i)
	{
		if(pdatas->sample(i).x()>=min 
			&&
			pdatas->sample(i).x()<= max)
		{
			out_xs.push_back(pdatas->sample(i).x());
			out_ys.push_back(pdatas->sample(i).y());
			if (firstIn)
			{
				boundary.rx() = i;
				firstIn = false;
			}
			boundary.ry() = i;
		}
	}
	return boundary;
}
///
/// \brief 获取当前正在显示的区域
/// \return
///
QRectF ChartWave_qwt::getPlottingRegionRang() const
{
    QwtPlot::Axis xaxis = QwtPlot::xBottom;
    if(!axisEnabled(QwtPlot::xBottom))
        xaxis = QwtPlot::xTop;
    QwtInterval inter = axisInterval(xaxis);
    double xmin = inter.minValue();
    double xmax = inter.maxValue();
    QwtPlot::Axis yaxis = QwtPlot::yLeft;
    if(!axisEnabled(QwtPlot::yLeft))
        yaxis = QwtPlot::yRight;
    inter = axisInterval(yaxis);
    double ymin = inter.minValue();
    double ymax = inter.maxValue();
    return QRectF(xmin,ymin,xmax-xmin,ymax-ymin);
}
///
/// \brief 把范围内的数据移除
/// \param removeRang 需要移除的数据范围
/// \param curve 需要移除数据的曲线
/// \return
///
size_t ChartWave_qwt::removeDataInRang(const QRectF& removeRang,QwtPlotCurve* curve)
{
    //由于vector不善于删除，因此此处为了提高效率，先进行一份拷贝，再进行替换
    size_t length = curve->data()->size();
    QVector<QPointF> newLine;
    newLine.reserve(length);
    QPointF point;
    for(size_t i = 0;i<length;++i)
    {
        point = curve->data()->sample(i);
        if(removeRang.contains(point))
            continue;
        newLine.push_back(point);
    }
    curve->setSamples(newLine);
    return newLine.size();
}
///
/// \brief 把AxisDateScaleType转换为字符
/// \param type 类型
/// \return
///
QString ChartWave_qwt::axisDateScaleType2String(AxisDateScaleType type)
{
	switch (type)
	{
	case ChartWave_qwt::h_m: return QString("h:m");
	case ChartWave_qwt::hh_mm: return QString("hh:mm");
	case ChartWave_qwt::h_m_s: return QString("h:m:s");
	case ChartWave_qwt::hh_mm_ss: return QString("hh:mm:ss");
	case ChartWave_qwt::yyyy_M_d: return QString("yyyy-M-d");
	case ChartWave_qwt::yyyy_M_d_h_m: return QString("yyyy-M-d h:m");
	case ChartWave_qwt::yyyy_M_d_h_m_s: return QString("yyyy-M-d h:m:s");
	case ChartWave_qwt::yyyy_MM_dd: return QString("yyyy-MM-dd");
	case ChartWave_qwt::yyyy_MM_dd_hh_mm: return QString("yyyy-MM-dd hh:mm");
	case ChartWave_qwt::yyyy_MM_dd_hh_mm_ss: return QString("yyyy-MM-dd hh:mm:ss");
	}
	return QString("yyyy-MM-dd hh:mm:ss");
}
///
/// \brief 设置坐标轴为时间坐标轴
/// \param axisID 轴的类型 
/// \param 时间显示的类型 
/// \param type 类型 
///
void ChartWave_qwt::setDateAxis(AxisDateScaleType type,int axisID,QwtDate::IntervalType intType)
{
	QString strDateFormat = axisDateScaleType2String(type);
	setDateAxis(strDateFormat,axisID,intType);
}

void ChartWave_qwt::setDateAxis(QString type,int axisID ,QwtDate::IntervalType intType)
{
	QwtDateScaleDraw* dateScale;
	dateScale = new QwtDateScaleDraw;
	dateScale->setDateFormat(intType,type);
	setAxisScaleDraw(axisID,dateScale);
}
