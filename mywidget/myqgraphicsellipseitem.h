#ifndef MYQGRAPHICSELLIPSEITEM_H
#define MYQGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QRectF>
#include <QWidget>

#include <any>

class MyQGraphicsEllipseItem : public QGraphicsEllipseItem
{
public:
    MyQGraphicsEllipseItem() = default;
    MyQGraphicsEllipseItem(double x,
                           double y,
                           double width = 5,
                           double height = 5,
                           QGraphicsItem* parent = nullptr);

    void setSize(const QSize& size);

    /**
     * @brief used to obtain objects related to this ellipse item
     */
    std::vector<std::any> relatedObjects;
};

#endif  // MYQGRAPHICSELLIPSEITEM_H
