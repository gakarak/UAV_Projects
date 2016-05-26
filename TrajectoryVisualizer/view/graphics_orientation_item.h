#ifndef GRAPHICS_ORIENTATION_ITEM_H
#define GRAPHICS_ORIENTATION_ITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>

namespace viewpkg{

    class GraphicsOrientationItem : public QGraphicsItemGroup
    {
    public:
        GraphicsOrientationItem(QPointF center = QPointF(0, 0), double axis_length = 1, QGraphicsItem *parent = 0);

        void setCenter(QPointF center);
        void setAxisLength(double length);

    private:
        void updateAxis();

        QGraphicsLineItem xAxis;
        QGraphicsLineItem yAxis;

        QPointF center;
        double axis_length;
    };

}

#endif // GRAPHICS_ORIENTATION_ITEM_H
