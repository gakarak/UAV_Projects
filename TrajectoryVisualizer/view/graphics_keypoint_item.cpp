#include "graphics_keypoint_item.h"

#include <QPen>
#include <QBrush>

using namespace std;
using namespace viewpkg;

GraphicsKeyPointItem::GraphicsKeyPointItem(QPointF center, double angle, double radius)
    : center(center), angle(angle), radius(radius)
{
    setColor(QColor(255, 0, 0));

    this->addToGroup(&circle);
    this->addToGroup(&direction);
}

void GraphicsKeyPointItem::setCenter(QPointF center)
{
    this->center = center;
    updateKeyPoint();
}

void GraphicsKeyPointItem::setRadius(double radius)
{
    this->radius = radius;
    updateKeyPoint();
}

void GraphicsKeyPointItem::setAngle(double angle)
{
    this->angle = angle;
    updateKeyPoint();
}

void GraphicsKeyPointItem::updateKeyPoint()
{
    circle.setRect(center.x() - radius, center.y() - radius, 2*radius, 2*radius);
    QPointF directed_pt = QPointF(radius * cos(angle*M_PI/180.), radius * sin(angle*M_PI/180.));

    direction.setLine(center.x(), center.y(), center.x() + directed_pt.x(), center.y() + directed_pt.y());
}

void GraphicsKeyPointItem::setColor(QColor color)
{
    circle.setPen(QPen(QBrush(color), 2, Qt::SolidLine));
    direction.setPen(QPen(QBrush(color), 2, Qt::SolidLine));
}
