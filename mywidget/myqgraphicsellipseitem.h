#ifndef MYQGRAPHICSELLIPSEITEM_H
#define MYQGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QRectF>
#include <QWidget>

class MyQGraphicsEllipseItem : public QGraphicsEllipseItem
{
public:
    MyQGraphicsEllipseItem() = default;
    MyQGraphicsEllipseItem(double x,
                           double y,
                           double width = 5,
                           double height = 5,
                           QGraphicsItem* parent = nullptr);

    void setCenterPos(const qreal posx, const qreal posy);
    void setSize(const QSize& size);
    QPointF centerPos();
};

#endif  // MYQGRAPHICSELLIPSEITEM_H
