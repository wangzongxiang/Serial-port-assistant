#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QtSerialPort>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void openserialport();
    void checkserialport();
    void readdata();
    void senddata();
    QSerialPort* serialport;
    QTimerEvent* timer;
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
