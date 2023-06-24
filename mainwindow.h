#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>

#include "dialog/newmap/newmapdialog.h"
#include "geometry/point.h"
#include "mywidget/clickgraphicsscene.h"
#include "voronoi/sweepline.h"
#include "voronoi/voronoi.h"

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

    void on_actionToggle_T_toggled(bool arg1);

    void on_actionStep_N_triggered();

private:
    Ui::MainWindow* ui;
    std::unique_ptr<ClickGraphicsScene> scene;
    std::shared_ptr<Voronoi> vmap;
    std::unique_ptr<QObject> vmapContext;
    std::shared_ptr<SweepLine> sl;
    bool autoFortune = false;

    void performAndSyncScene();
    void stepAndSyncScene();

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent* event);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent* event);
};
#endif  // MAINWINDOW_H
