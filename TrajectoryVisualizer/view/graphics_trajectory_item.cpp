#include "graphics_trajectory_item.h"

#include <QColor>
#include <QDebug>

#include "config_singleton.h"

using namespace viewpkg;
using namespace std;

GraphicsTrajectoryItem::GraphicsTrajectoryItem()
{
    trajectory_layer.setHandlesChildEvents(false);

    direction_layer.setZValue(1);
    orientation_layer.setZValue(2);
    key_point_layer.setZValue(3);

    this->setHandlesChildEvents(false);
    this->setAcceptHoverEvents(true);

    this->addToGroup(&trajectory_layer);
    this->addToGroup(&orientation_layer);
    this->addToGroup(&direction_layer);
    this->addToGroup(&key_point_layer);
}

void GraphicsTrajectoryItem::pushBackFrame(QPixmap img, QPointF center_coords_px, double angle, double meters_per_pixel, double quality)
{
    double m_per_px = ConfigSingleton::getInstance().getCommonMetersPerPixel();
    //setup map item
    shared_ptr<GraphicsFrameItem> frame_item = make_shared<GraphicsFrameItem>(img, frames.size()); //the frame_number is size

    makeTransforms(frame_item, frame_item->getMapItem().boundingRect().center(), center_coords_px, angle, meters_per_pixel);

    frames.push_back(frame_item);
    trajectory_layer.addToGroup(frames.back().get());

    //setup orientation
    shared_ptr<GraphicsOrientationItem> orient_item = make_shared<GraphicsOrientationItem>();
    orient_item->setCenter(frame_item->getMapItem().boundingRect().center());
    orient_item->setAxisLength(frame_item->getMapItem().boundingRect().width()/4.0);

    makeTransforms(orient_item, frame_item->getMapItem().boundingRect().center(), center_coords_px, angle, meters_per_pixel);

    orientations.push_back(orient_item);
    orientation_layer.addToGroup(orientations.back().get());

    double red_mul = quality < 0.5? 1.: 2*quality - 1;
    double green_mul = quality < 0.5? 2*quality: 1.;
    direction_layer.pushBackWayPoint(center_coords_px / (m_per_px / meters_per_pixel), QColor(int(255*red_mul), int(255*green_mul), 0));
}

void GraphicsTrajectoryItem::addKeyPoint(int frame_num, QPointF center_px, double angle, double radius, QColor color)
{
    shared_ptr<GraphicsFastKeyPointItem> key_point = make_shared<GraphicsFastKeyPointItem>();
    shared_ptr<GraphicsFrameItem> &frame_item = frames[frame_num];

    double scale = frame_item->transform().m11();
    QPointF item_center = center_px * scale;
    QPointF orig = QTransform().rotate(frame_item->rotation()).map(frame_item->transformOriginPoint());
    QPointF frame_center = frame_item->pos() + orig*scale;
    //key_point->setTransform(QTransform().translate(item_center - map->pos()).rotate(45).translate(map->pos() - item_center));

    key_point->setRadius(radius);
    key_point->setAngle(angle);
    key_point->setColor(color);

    key_point->setTransformOriginPoint((frame_center - item_center)/scale);
    key_point->setPos(item_center);
    key_point->setRotation(frame_item->rotation());
    key_point->setTransform(QTransform().translate(0, 0).scale(scale, scale).translate(0, 0));

    //makeTransforms(key_point.get(), QPointF(0, 0), center_px, map->rotation(), meters_per_pixel);
    //key_point_layer.addToGroup(new QGraphicsEllipseItem(map_center.x() - 10, map_center.y() - 10, 20, 20));
    //key_point_layer.addToGroup(new QGraphicsEllipseItem(item_center.x() - 5, item_center.y() - 5, 10, 10));

    key_points.push_back(key_point);
    key_point_layer.addToGroup(key_points.back().get());
    frames_num.push_back(frame_num);
}

void GraphicsTrajectoryItem::makeTransforms(shared_ptr<QGraphicsItem> item, QPointF item_center_px, QPointF scene_center_pos_px, double angle, double meters_per_pixel)
{
    double m_per_px = ConfigSingleton::getInstance().getCommonMetersPerPixel();
    //old transformations set
    //because of translate from qt4.6 is obsolete
    //item->setTransform(QTransform::fromTranslate(-item_center_px.x(), -item_center_px.y()), false);
    //item->setScale( meters_per_pixel / m_per_px );

    item->setTransformOriginPoint( item_center_px );
    item->setRotation(angle);
    item->setTransform(QTransform().translate(0, 0).scale(meters_per_pixel / m_per_px, meters_per_pixel / m_per_px).translate(0, 0), true);
    item->setPos( (scene_center_pos_px - item_center_px) / (m_per_px / meters_per_pixel) );
}

void GraphicsTrajectoryItem::setOrientationVisible(bool is_visible)
{
    orientation_layer.setVisible(is_visible);
}

void GraphicsTrajectoryItem::setTrajectoryVisible(bool is_visible)
{
    trajectory_layer.setVisible(is_visible);
}

void GraphicsTrajectoryItem::setDirectionVisible(bool is_visible)
{
    direction_layer.setVisible(is_visible);
}

void GraphicsTrajectoryItem::setKeyPointsVisible(bool is_visible)
{
    key_point_layer.setVisible(is_visible);
}

void GraphicsTrajectoryItem::clear()
{
    frames.clear();
    orientations.clear();
    direction_layer.clear();

    clearKeyPoints();
}

void GraphicsTrajectoryItem::clearKeyPoints()
{
    key_points.clear();
    frames_num.clear();
}

void GraphicsTrajectoryItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    this->setZValue(1);
    QGraphicsItemGroup::hoverEnterEvent(event);
}

void GraphicsTrajectoryItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->setZValue(0);
    QGraphicsItemGroup::hoverLeaveEvent(event);
}
