#include "graphics_fast_keypoint_item.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

using namespace viewpkg;

GraphicsFastKeyPointItem::GraphicsFastKeyPointItem(QPointF center, double angle, double radius, QGraphicsItem *parent)
    : QGraphicsItem(parent), center(center), angle(angle), radius(radius)
{

}

void GraphicsFastKeyPointItem::setCenter(QPointF center)
{
    this->center = center;
}

void GraphicsFastKeyPointItem::setRadius(double radius)
{
    this->radius = radius;
}

void GraphicsFastKeyPointItem::setAngle(double angle)
{
    this->angle = angle;
}

QRectF GraphicsFastKeyPointItem::boundingRect() const
{
    return QRectF(center.x() - radius, center.y() - radius, 2*radius, 2*radius);
}

void GraphicsFastKeyPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(QBrush(color), 0.5, Qt::SolidLine));

    painter->drawEllipse(center.x() - radius, center.y() - radius, 2*radius, 2*radius);

    QPointF directed_pt = QPointF(radius * cos(angle*M_PI/180.), radius * sin(angle*M_PI/180.));
    painter->drawLine(center.x(), center.y(), center.x() + directed_pt.x(), center.y() + directed_pt.y());
}

void GraphicsFastKeyPointItem::setColor(QColor color)
{
    this->color = color;
}
