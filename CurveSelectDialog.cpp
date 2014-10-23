#include "CurveSelectDialog.h"
#include <QItemSelectionModel>
#include <QMessageBox>

void myUI::CurveSelectDialog::setupUi(QDialog *d)
{
    if (d->objectName().isEmpty())
        d->setObjectName(QStringLiteral("CurveSelectDialog"));
    d->resize(587, 383);
    verticalLayout = new QVBoxLayout(d);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    tableView = new QTableView(d);
    tableView->setObjectName(QStringLiteral("tableView"));
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSortingEnabled(true);

    verticalLayout->addWidget(tableView);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    pushButton_reject = new QPushButton(d);
    pushButton_reject->setObjectName(QStringLiteral("pushButton_reject"));

    horizontalLayout->addWidget(pushButton_reject);

    pushButton_ok = new QPushButton(d);
    pushButton_ok->setObjectName(QStringLiteral("pushButton_ok"));

    horizontalLayout->addWidget(pushButton_ok);


    verticalLayout->addLayout(horizontalLayout);


    d->setWindowTitle(QApplication::translate("CurveSelectDialog", "\346\233\262\347\272\277\351\200\211\346\213\251", 0));
    pushButton_reject->setText(QApplication::translate("CurveSelectDialog", "\345\217\226 \346\266\210", 0));
    pushButton_ok->setText(QApplication::translate("CurveSelectDialog", "\347\241\256 \345\256\232", 0));

    QObject::connect(pushButton_reject, SIGNAL(clicked()), d, SLOT(reject()));

    QMetaObject::connectSlotsByName(d);
}

CurveSelectDialog::CurveSelectDialog(SAPlotChart* chart ,QWidget *parent) :
    QDialog(parent)
	,m_chart(chart)
{
    ui = std::make_shared<myUI::CurveSelectDialog>();
    ui->setupUi(this);
	QStandardItemModel* model = new QStandardItemModel(ui->tableView);
	model->setHorizontalHeaderLabels(QStringList()
		<<QStringLiteral("曲线名称")
		<<QStringLiteral("颜色")
		<<QStringLiteral("数据点数"));
	QList<QwtPlotCurve*> listCur = m_chart->getCurveList();
	QStandardItem* item = nullptr;
	for (int i=0;i<listCur.size();++i)
	{

		item = new QStandardItem(listCur[i]->title().text());
		model->setItem(i,0,item);

		item = new QStandardItem;
		item->setData(listCur[i]->pen().color(),Qt::BackgroundRole);
		model->setItem(i,1,item);

		item = new QStandardItem(QStringLiteral("%1").arg(listCur[i]->data()->size()));
		model->setItem(i,2,item);
	}
	ui->tableView->setModel(model);
}

CurveSelectDialog::~CurveSelectDialog()
{
}

QStandardItemModel* CurveSelectDialog::getTableModel()
{
	return static_cast<QStandardItemModel*>(ui->tableView->model());
}

void CurveSelectDialog::on_pushButton_ok_clicked()
{
	QItemSelectionModel *sel = ui->tableView->selectionModel();
	QModelIndexList indexList = sel->selectedRows();
	if (indexList.size()<=0)
	{
		QMessageBox::information(this,QStringLiteral("通知"),QStringLiteral("请选择曲线"));
		return;
	}
	QList<QwtPlotCurve*> allCurve = m_chart->getCurveList();
	for(int i=0;i<indexList.size();++i)
	{
		m_selCurve.append(
			allCurve[ indexList[i].row() ]
		);
	}
	QDialog::accept();
}
