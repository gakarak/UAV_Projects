#ifndef GRAPHICSFRAMEITEM_H
#define GRAPHICSFRAMEITEM_H

#include "graphics_map_item.h"

namespace viewpkg
{

    class GraphicsFrameItem : public QObject, public GraphicsMapItem
    {
        Q_OBJECT

    public:
        GraphicsFrameItem(QGraphicsItem *parent = 0);
        GraphicsFrameItem(const QPixmap &frame, int frame_num, QGraphicsItem *parent = 0);

        void setFrameNumber(int frame_num)  { this->frame_num = frame_num; }
        int  getFrameNumber() const         { return frame_num; }

        bool isSelected() const             { return selected; }

    signals:
        void stateChanged(int frame_num, bool isSelected);

    protected:
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

        //if selected then hover events cannot hide bound_item
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    private:
        int frame_num;

        bool selected;
    };

}

#endif // GRAPHICSFRAMEITEM_H
