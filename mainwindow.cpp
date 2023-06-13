#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->graphicsView->setBackgroundBrush(QBrush(QColor(135, 206, 235)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_Map_triggered()
{
    NewMapDialog nmd;
    nmd.exec();

    scene.reset(new ClickGraphicsScene(nmd.size));
    ui->graphicsView->setScene(scene.get());
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    ui->graphicsView->setGeometry(1, 1, this->centralWidget()->width() - 2,
                                  this->centralWidget()->height() - 4);

    if (scene != nullptr)
        ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
