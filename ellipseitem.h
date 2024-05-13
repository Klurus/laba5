#ifndef ELLIPSEITEM_H
#define ELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QColor>

class EllipseItem : public QGraphicsEllipseItem
{
public:
    EllipseItem(QRectF rect, QColor color, QGraphicsItem *parent = nullptr);
    void setColor(QColor color);

private:
    QColor m_color;
};

#endif // ELLIPSEITEM_H
