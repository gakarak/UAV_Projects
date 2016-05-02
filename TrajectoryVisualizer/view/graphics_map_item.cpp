#include "graphics_map_item.h"

#include <QPen>
#include <QBrush>
#include <QDebug>

using namespace viewpkg;

GraphicsMapItem::GraphicsMapItem() : QGraphicsItemGroup()//, is_hoverable(true)
{
    bound_item.setPen(QPen(QBrush(QColor(255, 0, 0)), 8, Qt::SolidLine));
    bound_item.setVisible(false);

    //orient_item.setPen(QPen(QBrush(QColor(255, 0, 0)), 8, Qt::SolidLine));
    //orient_item.;

    map_item.setAcceptHoverEvents(true);

    this->addToGroup(&map_item);
    this->addToGroup(&bound_item);
    //this->addToGroup(&orient_item);
    this->setHandlesChildEvents(true);
}

GraphicsMapItem::GraphicsMapItem(const QPixmap &map) : GraphicsMapItem()
{
    setMapItem(map);
}

void GraphicsMapItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    bound_item.setVisible(true);
    this->setZValue(1);
}

void GraphicsMapItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    bound_item.setVisible(false);
    this->setZValue(0);
}

void GraphicsMapItem::setMapItem(const QPixmap &pixmap)
{
    map_item.setPixmap(pixmap);
    bound_item.setRect(map_item.boundingRect());
    //orient_item.setCenter(map_item.boundingRect().center());
    //orient_item.setAxisLength(map_item.boundingRect().width()/4.0);
}

void GraphicsMapItem::setHoverable(bool is_hoverable)
{
    //this->is_hoverable = is_hoverable;
    map_item.setAcceptHoverEvents(is_hoverable);
}
