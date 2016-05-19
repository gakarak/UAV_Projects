#ifndef GRAPHICSFRAMEITEM_H
#define GRAPHICSFRAMEITEM_H

#include "graphics_map_item.h"

namespace viewpkg
{

    class GraphicsFrameItem : public GraphicsMapItem
    {
    public:
        GraphicsFrameItem();
        GraphicsFrameItem(const QPixmap &frame, int frame_num);

        void setFrameNumber(int frame_num)  { this->frame_num = frame_num; }
        int  getFrameNumber()               { return frame_num; }

    private:
        int frame_num;
    };

}

#endif // GRAPHICSFRAMEITEM_H
