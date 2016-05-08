#ifndef GRAPHICS_DIRECTION_ITEM_H
#define GRAPHICS_DIRECTION_ITEM_H

#include <vector>
#include <memory>

#include <QGraphicsItemGroup>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QColor>

namespace viewpkg
{

    class GraphicsDirectionItem : public QGraphicsItemGroup
    {
    public:
        GraphicsDirectionItem();

        void pushBackWayPoint(QPointF scene_center_pos_px, QColor brush_color = QColor(0, 0, 0));
        void clear();

    private:
        std::vector<std::shared_ptr<QGraphicsEllipseItem>>  points;
        std::vector<std::shared_ptr<QGraphicsLineItem>>     edges;
    };

}

#endif // GRAPHICS_DIRECTION_ITEM_H
