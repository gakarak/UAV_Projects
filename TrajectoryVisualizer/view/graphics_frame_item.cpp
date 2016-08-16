#include "graphics_frame_item.h"

#include <QDebug>

using namespace viewpkg;

GraphicsFrameItem::GraphicsFrameItem(QGraphicsItem *parent)
  : GraphicsMapItem(parent), selected(false)
{
}

GraphicsFrameItem::GraphicsFrameItem(const QPixmap &frame, int frame_num,
                                     QGraphicsItem *parent)
  : GraphicsMapItem(frame, parent), frame_num(frame_num), selected(false)
{
}

void GraphicsFrameItem::setSelected(bool value)
{
  if (selected != value)
  {
    selected = value;

    //selected item always have bounds
    bound_item.setVisible(selected);

    emit stateChanged(frame_num, selected);
  }
}

void GraphicsFrameItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  selected = !selected;

  //selected item always have bounds
  bound_item.setVisible(selected);

  emit stateChanged(frame_num, selected);
}

void GraphicsFrameItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  qDebug() << "hovered: " << frame_num;
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
