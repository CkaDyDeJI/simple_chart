#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class PlainChart;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void doSomething();

public slots:
    void setCoords(double x, double y);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    PlainChart* chart;
    QLabel* label;
};
#endif // MAINWINDOW_H
