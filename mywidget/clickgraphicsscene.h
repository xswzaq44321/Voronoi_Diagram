#ifndef CLICKGRAPHICSSCENE_H
#define CLICKGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

#include <QDebug>

#include "geometry/polygon.h"
#include "myqgraphicsellipseitem.h"

class ClickGraphicsScene : public QGraphicsScene
{
public:
    explicit ClickGraphicsScene(QObject* parent = nullptr);
    ClickGraphicsScene(const QSize& size);

    std::unique_ptr<QPixmap> mapCanvas;
    QVector<std::shared_ptr<MyQGraphicsEllipseItem>> ellipseItems;
    QVector<std::shared_ptr<QGraphicsLineItem>> lineItems;
    QVector<std::shared_ptr<QGraphicsItem>> assistantItems;

    void addPoint(QPointF pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    std::unique_ptr<QGraphicsPixmapItem> mapCanvasItem;
    QPointF MousePrevPoint;
    MyQGraphicsEllipseItem* draggedObj = nullptr;
};

#endif  // CLICKGRAPHICSSCENE_H
