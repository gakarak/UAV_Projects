#include "graphics_trajectory_item.h"

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

void GraphicsTrajectoryItem::pushBackMap(QPixmap map_img, QPointF center_coords_px, double angle, double meters_per_pixel)
{
    double m_per_px = ConfigSingleton::getInstance().getCommonMetersPerPixel();
    //setup map item
    shared_ptr<GraphicsMapItem> map_item = make_shared<GraphicsMapItem>(map_img);

    makeTransforms(map_item, map_item->getMapItem().boundingRect().center(), center_coords_px, angle, meters_per_pixel);

    trajectory.push_back(map_item);
    trajectory_layer.addToGroup(trajectory.back().get());

    //setup orientation
    shared_ptr<GraphicsOrientationItem> orient_item = make_shared<GraphicsOrientationItem>();
    orient_item->setCenter(map_item->getMapItem().boundingRect().center());
    orient_item->setAxisLength(map_item->getMapItem().boundingRect().width()/4.0);

    makeTransforms(orient_item, map_item->getMapItem().boundingRect().center(), center_coords_px, angle, meters_per_pixel);

    orientations.push_back(orient_item);
    orientation_layer.addToGroup(orientations.back().get());

    direction_layer.pushBackWayPoint(center_coords_px / (m_per_px / meters_per_pixel));
}

void GraphicsTrajectoryItem::addKeyPoint(int map_num, QPointF center_px, double angle, double radius, QColor color)
{
    shared_ptr<GraphicsKeyPointItem> key_point = make_shared<GraphicsKeyPointItem>();
    shared_ptr<GraphicsMapItem> &map = trajectory[map_num];

    double scale = map->transform().m11();
    QPointF item_center = center_px * scale;
    QPointF orig = QTransform().rotate(map->rotation()).map(map->transformOriginPoint());
    QPointF map_center = map->pos() + orig*scale;
    //key_point->setTransform(QTransform().translate(item_center - map->pos()).rotate(45).translate(map->pos() - item_center));

    key_point->setRadius(radius);
    key_point->setAngle(angle);
    key_point->setColor(color);

    //qDebug() << "pos " << item_center - map_center;

    key_point->setTransformOriginPoint((map_center - item_center)/scale);
    key_point->setPos(item_center);
    key_point->setRotation(map->rotation());
    key_point->setTransform(QTransform().translate(0, 0).scale(scale, scale).translate(0, 0));

    //makeTransforms(key_point.get(), QPointF(0, 0), center_px, map->rotation(), meters_per_pixel);
    //key_point_layer.addToGroup(new QGraphicsEllipseItem(map_center.x() - 10, map_center.y() - 10, 20, 20));
    //key_point_layer.addToGroup(new QGraphicsEllipseItem(item_center.x() - 5, item_center.y() - 5, 10, 10));

    //qDebug() << "Transform: " << map->transformOriginPoint();

    key_points.push_back(key_point);
    key_point_layer.addToGroup(key_points.back().get());
    maps_num.push_back(map_num);
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
    trajectory.clear();
    orientations.clear();
    direction_layer.clear();

    clearKeyPoints();
}

void GraphicsTrajectoryItem::clearKeyPoints()
{
    key_points.clear();
    maps_num.clear();
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
