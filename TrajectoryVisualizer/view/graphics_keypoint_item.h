#ifndef GRAPHICS_KEYPOINT_ITEM_H
#define GRAPHICS_KEYPOINT_ITEM_H

#include <QGraphicsItemGroup>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QColor>

namespace viewpkg
{

    class GraphicsKeyPointItem : public QGraphicsItemGroup
    {
    public:
        GraphicsKeyPointItem(QPointF center = QPointF(0, 0), double angle = 0, double radius = 1);

        void setCenter(QPointF center);
        void setRadius(double radius);
        void setColor(QColor color);
        void setAngle(double angle);

    private:
        void updateKeyPoint();

        QGraphicsEllipseItem    circle;
        QGraphicsLineItem       direction;

        QPointF center;
        double radius;
        double angle;
    };

}
#endif // GRAPHICS_KEYPOINT_ITEM_H
