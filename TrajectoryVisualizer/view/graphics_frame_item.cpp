#include "graphics_frame_item.h"

using namespace viewpkg;

GraphicsFrameItem::GraphicsFrameItem()
    : GraphicsMapItem()
{
}

GraphicsFrameItem::GraphicsFrameItem(const QPixmap &frame, int frame_num)
    : GraphicsMapItem(frame), frame_num(frame_num)
{
}
