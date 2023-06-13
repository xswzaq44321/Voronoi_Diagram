#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "dialog/newmap/newmapdialog.h"
#include "geometry/point.h"
#include "mywidget/clickgraphicsscene.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_Map_triggered();

private:
    Ui::MainWindow* ui;
    std::shared_ptr<ClickGraphicsScene> scene;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent* event);
};
#endif  // MAINWINDOW_H
