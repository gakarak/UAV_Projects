#include "graphics_frame_item.h"

#include <QDebug>

using namespace viewpkg;

GraphicsFrameItem::GraphicsFrameItem(QGraphicsItem *parent)
    : GraphicsMapItem(parent), selected(false)
{
}

GraphicsFrameItem::GraphicsFrameItem(const QPixmap &frame, int frame_num, QGraphicsItem *parent)
    : GraphicsMapItem(frame, parent), frame_num(frame_num), selected(false)
{
}

void GraphicsFrameItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    selected = !selected;

    //selected item always have bounds
    bound_item.setVisible(selected);
}

void GraphicsFrameItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (!selected)
    {
        GraphicsMapItem::hoverEnterEvent(event);
    }
}

void GraphicsFrameItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!selected)
    {
        GraphicsMapItem::hoverLeaveEvent(event);
    }
}
