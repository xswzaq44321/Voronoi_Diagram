#include "clickgraphicsscene.h"

ClickGraphicsScene::ClickGraphicsScene(QObject* parent)
    : QGraphicsScene{parent}
{
}

ClickGraphicsScene::ClickGraphicsScene(const QSize& size)
{
    qDebug("size: %d %d\n", size.width(), size.height());
    mapCanvas = std::make_unique<QPixmap>(size.width(), size.height());
    mapCanvas->fill();
    this->setSceneRect(0, 0, size.width(), size.height());
    mapCanvasItem.reset(this->addPixmap(*mapCanvas));
    mapCanvasItem->setZValue(-10000);
}

void ClickGraphicsScene::addPoint(QPointF pos)
{
    if (!this->sceneRect().contains(pos))
        return;
    auto item =
        std::make_shared<MyQGraphicsEllipseItem>(pos.x(), pos.y(), 5, 5);
    item->setPos(item->pos());
    this->addItem(item.get());
    ellipseItems.push_back(std::move(item));
}

void ClickGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    auto tmp = this->itemAt(event->scenePos(), QTransform());
    auto item = dynamic_cast<MyQGraphicsEllipseItem*>(tmp);
    if (event->button() == Qt::LeftButton) {
        if (item != nullptr) {
            MousePrevPoint = event->scenePos();
            draggedObj = item;
        } else {
            this->addPoint(event->scenePos());
        }
    } else if (event->button() == Qt::RightButton) {
        if (item == nullptr || draggedObj)
            return;
        this->removeItem(item);
        ellipseItems.erase(
            std::find_if(ellipseItems.cbegin(), ellipseItems.cend(),
                         [&item](const auto& a) { return a.get() == item; }));
    }
    qDebug("point cnt:%d\n", ellipseItems.size());
}

void ClickGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (draggedObj != nullptr) {
        QPointF newPos =
            draggedObj->centerPos() + (event->scenePos() - MousePrevPoint);
        if (this->sceneRect().contains(newPos)) {
            draggedObj->setCenterPos(newPos.x(), newPos.y());
        }
        MousePrevPoint = event->scenePos();
    }
}

void ClickGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
    draggedObj = nullptr;
    MousePrevPoint.setX(0);
    MousePrevPoint.setY(0);
}
