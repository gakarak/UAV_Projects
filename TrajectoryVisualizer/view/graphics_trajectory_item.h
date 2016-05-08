#ifndef GRAPHICS_TRAJECTORY_ITEM_H
#define GRAPHICS_TRAJECTORY_ITEM_H

#include <vector>
#include <memory>

#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QColor>

#include "graphics_map_item.h"
#include "graphics_orientation_item.h"
#include "graphics_direction_item.h"
#include "graphics_keypoint_item.h"

namespace viewpkg
{

    class GraphicsTrajectoryItem : public QGraphicsItemGroup
    {
    public:
        GraphicsTrajectoryItem();

        void pushBackMap(QPixmap map_img, QPointF center_coords_px, double angle, double meters_per_pixel, double quality);
        void addKeyPoint(int map_num, QPointF center_px, double angle, double radius, QColor color);
        void clear();
        void clearKeyPoints();

        void setTrajectoryVisible(bool is_visible);
        void setOrientationVisible(bool is_visible);
        void setDirectionVisible(bool is_visible);
        void setKeyPointsVisible(bool is_visible);

        //double getMetersPerPixel() const { return m_per_px; }

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    private:
        void makeTransforms(std::shared_ptr<QGraphicsItem> item, QPointF item_center_px, QPointF scene_center_pos_px, double angle, double meters_per_pixel);

        QGraphicsItemGroup trajectory_layer;
        GraphicsDirectionItem direction_layer;
        QGraphicsItemGroup orientation_layer;
        QGraphicsItemGroup key_point_layer;

        std::vector<std::shared_ptr<GraphicsMapItem>> trajectory;
        std::vector<std::shared_ptr<GraphicsOrientationItem>> orientations;

        std::vector<std::shared_ptr<GraphicsKeyPointItem>> key_points;
        std::vector<int> maps_num;

        //std::vector<std::shared_ptr<QGraphicsEllipseItem>>

        //double m_per_px;
    };

}

#endif // GRAPHICS_TRAJECTORY_ITEM_H
