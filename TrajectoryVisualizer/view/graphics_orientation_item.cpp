#include "graphics_orientation_item.h"

#include <QPen>
#include <QBrush>

using namespace viewpkg;

GraphicsOrientationItem::GraphicsOrientationItem(QPointF center, double length, QGraphicsItem *parent)
    : QGraphicsItemGroup(parent), center(center), axis_length(length)
{
    xAxis.setPen(QPen(QBrush(QColor(255, 0, 0)), 8, Qt::SolidLine));
    yAxis.setPen(QPen(QBrush(QColor(0, 255, 0)), 8, Qt::SolidLine));

    updateAxis();

    this->addToGroup(&xAxis);
    this->addToGroup(&yAxis);
}

void GraphicsOrientationItem::updateAxis()
{
    xAxis.setLine(center.x(), center.y(), center.x() + axis_length, center.y());
    yAxis.setLine(center.x(), center.y(), center.x(), center.y() - axis_length);
}

void GraphicsOrientationItem::setCenter(QPointF center)
{
    this->center = center;
    updateAxis();
}

void GraphicsOrientationItem::setAxisLength(double length)
{
    axis_length = length;
    updateAxis();
}
