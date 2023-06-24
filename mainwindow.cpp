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

    scene = std::make_unique<ClickGraphicsScene>(nmd.size);
    vmap = std::make_shared<Voronoi>(nmd.size.width(), nmd.size.height());
    vmapContext = std::make_unique<QObject>();
    sl.reset();
    ui->graphicsView->setScene(scene.get());
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    std::weak_ptr<Voronoi> weak_vmap = vmap;
    connect(scene.get(), &ClickGraphicsScene::pointAdded, vmapContext.get(),
            [weak_vmap](MyQGraphicsEllipseItem* e) {
                std::shared_ptr<Voronoi> vmap = weak_vmap.lock();
                if (!vmap)
                    return;
                auto newPoly = vmap->addPoly(Polygon(e->x(), e->y()));
                std::weak_ptr<Polygon> tmp = *newPoly;
                e->relatedObjects.push_back(tmp);
            });
    connect(scene.get(), &ClickGraphicsScene::pointMoved, vmapContext.get(),
            [](MyQGraphicsEllipseItem* e) {
                for (const std::any& obj : e->relatedObjects) {
                    if (obj.type() != typeid(std::weak_ptr<Polygon>))
                        continue;
                    auto weak_poly = std::any_cast<std::weak_ptr<Polygon>>(obj);
                    std::shared_ptr<Polygon> poly = weak_poly.lock();
                    if (!poly)
                        continue;
                    Point focus(e->x(), e->y());
                    poly->focus = focus;
                }
            });
    connect(scene.get(), &ClickGraphicsScene::pointRemoved, vmapContext.get(),
            [weak_vmap](MyQGraphicsEllipseItem* e) {
                std::shared_ptr<Voronoi> vmap = weak_vmap.lock();
                if (!vmap)
                    return;
                for (const std::any& obj : e->relatedObjects) {
                    if (obj.type() != typeid(std::weak_ptr<Polygon>))
                        continue;
                    auto weak_poly = std::any_cast<std::weak_ptr<Polygon>>(obj);
                    std::shared_ptr<Polygon> poly = weak_poly.lock();
                    if (!poly)
                        continue;
                    vmap->erasePoly(poly);
                }
            });

    auto autoPerform = [this]() {
        if (autoFortune)
            performAndSyncScene();
    };
    connect(scene.get(), &ClickGraphicsScene::pointAdded, this, autoPerform);
    connect(scene.get(), &ClickGraphicsScene::pointMoved, this, autoPerform);
    connect(scene.get(), &ClickGraphicsScene::pointRemoved, this, autoPerform);
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    ui->graphicsView->setGeometry(1, 1, this->centralWidget()->width() - 2,
                                  this->centralWidget()->height() - 4);

    if (scene != nullptr)
        ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::on_actionToggle_T_toggled(bool arg1)
{
    autoFortune = arg1;
    if (arg1) {
        performAndSyncScene();
    }
}

void MainWindow::performAndSyncScene()
{
    if (!vmap)
        return;
    if (!sl)
        sl = std::make_shared<SweepLine>();

    sl->loadVmap(vmap);
    sl->performFortune();

    for (const auto& line : scene->lineItems) {
        scene->removeItem(line.get());
    }
    scene->lineItems.clear();
    for (const auto& poly_sptr : sl->vmap->polygons) {
        for (const auto& edge_sptr : poly_sptr->edges) {
            const Edge& edge = *edge_sptr;
            if (*edge.a == *edge.b)
                continue;
            auto line = std::make_shared<QGraphicsLineItem>(
                edge.a->x, edge.a->y, edge.b->x, edge.b->y);
            scene->addItem(line.get());
            scene->lineItems.push_back(line);
        }
    }
}

void MainWindow::stepAndSyncScene()
{
    if (!vmap)
        return;
    if (!sl)
        sl = std::make_shared<SweepLine>(vmap);

    double L = sl->nextEvent();
    if (L == sl->LMAXVALUE)
        sl->finishEdges();

    for (const auto& line : scene->lineItems) {
        scene->removeItem(line.get());
    }
    scene->lineItems.clear();
    for (const auto& poly_sptr : sl->vmap->polygons) {
        for (const auto& edge_sptr : poly_sptr->edges) {
            const Edge& edge = *edge_sptr;
            if (!(edge.a && edge.b))
                continue;
            auto line = std::make_shared<QGraphicsLineItem>(
                edge.a->x, edge.a->y, edge.b->x, edge.b->y);
            scene->addItem(line.get());
            scene->lineItems.push_back(line);
        }
    }
}

void MainWindow::on_actionStep_N_triggered()
{
    stepAndSyncScene();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_T:
        ui->actionToggle_T->toggle();
        break;
    case Qt::Key_N:
        ui->actionStep_N->trigger();
    }
}
