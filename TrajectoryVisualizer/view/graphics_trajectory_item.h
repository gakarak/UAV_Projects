#ifndef GRAPHICS_TRAJECTORY_ITEM_H
#define GRAPHICS_TRAJECTORY_ITEM_H

#include <vector>
#include <memory>

#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QColor>

#include "graphics_frame_item.h"
#include "graphics_orientation_item.h"
#include "graphics_direction_item.h"
#include "graphics_keypoint_item.h"
#include "graphics_fast_keypoint_item.h"

namespace viewpkg
{

    class GraphicsTrajectoryItem : public QObject, public QGraphicsItemGroup
    {
        Q_OBJECT

    public:
        GraphicsTrajectoryItem();

        void pushBackFrame(QPixmap img, QPointF center_coords_px, double angle, double meters_per_pixel, double quality);
        void addKeyPoint(int frame_num, QPointF center_px, double angle, double radius, QColor color);
        void addKeyPointNew(QPointF pos, double angle, double radius, double scale, QColor color);

        void clear();
        void clearKeyPoints();

        void setTrajectoryVisible(bool is_visible);
        void setOrientationVisible(bool is_visible);
        void setDirectionVisible(bool is_visible);
        void setKeyPointsVisible(bool is_visible);

        //double getMetersPerPixel() const { return m_per_px; }
    public slots:
        void frameStateChanged(int frame_num, bool isSelected);
        void cleanSelection();

    signals:
        void frameDoubleClicked(int frame_num, bool isSelected);

    protected:
        void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    private:
        void makeTransforms(std::shared_ptr<QGraphicsItem> item, QPointF item_center_px, QPointF scene_center_pos_px, double angle, double meters_per_pixel);

        QGraphicsItemGroup trajectory_layer;
        GraphicsDirectionItem direction_layer;
        QGraphicsItemGroup orientation_layer;
        QGraphicsItemGroup key_point_layer;

        std::vector<std::shared_ptr<GraphicsFrameItem>> frames;
        std::vector<std::shared_ptr<GraphicsOrientationItem>> orientations;

        std::vector<std::shared_ptr<GraphicsFastKeyPointItem>> key_points;
        std::vector<int> frames_num;

        //std::vector<std::shared_ptr<QGraphicsEllipseItem>>

        //double m_per_px;
    };

}

#endif // GRAPHICS_TRAJECTORY_ITEM_H
