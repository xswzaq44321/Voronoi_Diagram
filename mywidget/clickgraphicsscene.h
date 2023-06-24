#ifndef CLICKGRAPHICSSCENE_H
#define CLICKGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

#include <QDebug>

#include "myqgraphicsellipseitem.h"

class ClickGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit ClickGraphicsScene(QObject* parent = nullptr);
    ClickGraphicsScene(const QSize& size);

    std::unique_ptr<QPixmap> mapCanvas;
    std::vector<std::shared_ptr<MyQGraphicsEllipseItem>> ellipseItems;
    std::vector<std::shared_ptr<QGraphicsLineItem>> lineItems;
    std::vector<std::shared_ptr<QGraphicsItem>> assistantItems;

    void addPoint(const QPointF& pos);

signals:
    void pointAdded(MyQGraphicsEllipseItem*);
    void pointMoved(MyQGraphicsEllipseItem*);
    void pointRemoved(MyQGraphicsEllipseItem*);

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
