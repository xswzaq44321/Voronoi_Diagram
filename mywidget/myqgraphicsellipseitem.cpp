#include "myqgraphicsellipseitem.h"

MyQGraphicsEllipseItem::MyQGraphicsEllipseItem(double x,
                                               double y,
                                               double width,
                                               double height,
                                               QGraphicsItem* parent)
    : QGraphicsEllipseItem(0, 0, width, height, parent)
{
    this->setBrush(Qt::black);
    this->setRect(-width / 2, -height / 2, width, height);
    this->setPos(x, y);
}

void MyQGraphicsEllipseItem::setSize(const QSize& size)
{
    double width = size.width();
    double height = size.height();
    this->setRect(-width / 2, -height / 2, width, height);
}
