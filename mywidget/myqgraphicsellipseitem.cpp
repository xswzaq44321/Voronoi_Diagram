#include "myqgraphicsellipseitem.h"

MyQGraphicsEllipseItem::MyQGraphicsEllipseItem(double x,
                                               double y,
                                               double width,
                                               double height,
                                               QGraphicsItem* parent)
    : QGraphicsEllipseItem(0, 0, width, height, parent)
{
    this->setBrush(Qt::black);
    QGraphicsEllipseItem::setPos(x - width / 2, y - height / 2);
}

void MyQGraphicsEllipseItem::setCenterPos(const qreal posx, const qreal posy)
{
    double x = posx - this->rect().width() / 2;
    double y = posy - this->rect().height() / 2;
    QGraphicsEllipseItem::setPos(x, y);
}

void MyQGraphicsEllipseItem::setSize(const QSize& size)
{
    double width = size.width();
    double height = size.height();
    double x = this->x() - width / 2;
    double y = this->y() - height / 2;
    this->setRect(0, 0, width, height);
}

QPointF MyQGraphicsEllipseItem::centerPos()
{
    return this->pos() +
           QPointF(this->rect().width() / 2, this->rect().height() / 2);
}
