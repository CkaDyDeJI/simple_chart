#include "mainwindow.h"
#include "plainchart.h"

#include <QLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      chart(new PlainChart(this)),
      label(new QLabel(this))
{
    label->setGeometry(0, 0, 200, label->size().height());
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(chart, SIGNAL(currentCoords(double,double)), this, SLOT(setCoords(double,double)));

    setGeometry(0, 0, 700, 300);

    chart->setGeometry(geometry());
    chart->setDefaultPostition(PlainChart::BottomLeft);

    chart->xAxis()->grid()->setZeroLinePen(QPen(Qt::red, 1, Qt::SolidLine));
    chart->yAxis()->grid()->setZeroLinePen(QPen(Qt::red, 1, Qt::SolidLine));

    QVBoxLayout* lay = new QVBoxLayout;
    lay->addWidget(chart);
    lay->addWidget(label);

    layout()->addItem(lay);
}

MainWindow::~MainWindow()
{
}

void MainWindow::doSomething()
{
    QVector<QPointF> vec;
    for (int i = 0; i < 5; i++)
    {
        const int sign = (i % 2 == 0) ? 1 : -1;
        vec.append(QPointF(i - 3, i + sign * 0.5));
    }

    ChartDataItem* data = chart->createDataItem(spline);
    data->setData(vec);

    chart->rescaleAxes();
    chart->replot();
}

void MainWindow::setCoords(double x, double y)
{
    label->setText(QString("x = %1, y = %2").arg(x).arg(y));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    chart->setGeometry(geometry());
}

