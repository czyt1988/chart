#ifndef DIALOG_SELCURVE_H
#define DIALOG_SELCURVE_H

#include <QDialog>
#include "MdiSubWindow.h"
#include "SAChartSupport.h"
#include <QList>
#include <QStandardItemModel>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <memory>
namespace myUI {
class CurveSelectDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTableView *tableView;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_reject;
    QPushButton *pushButton_ok;
    void setupUi(QDialog* d);
};
}

class CurveSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CurveSelectDialog(SAPlotChart* chart ,QWidget *parent = 0);
    ~CurveSelectDialog();
    QList<QwtPlotCurve*> getSelCurve()
	{
		return m_selCurve;
	}
private slots:
	void on_pushButton_ok_clicked();
private:
    std::shared_ptr<myUI::CurveSelectDialog> ui;
	SAPlotChart* m_chart;
	QList<QwtPlotCurve*> m_selCurve;
	QStandardItemModel* getTableModel();
};

#endif // DIALOG_SELCURVE_H
