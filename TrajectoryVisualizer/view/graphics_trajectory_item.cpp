#include "graphics_trajectory_item.h"

#include <QColor>
#include <QDebug>

#include "config_singleton.h"

using namespace viewpkg;
using namespace std;

GraphicsTrajectoryItem::GraphicsTrajectoryItem()
  : trajectory_layer(this), orientation_layer(this),
    direction_layer(this), key_point_layer(this)
{
  trajectory_layer.setHandlesChildEvents(false);

  direction_layer.setZValue(1);
  orientation_layer.setZValue(2);
  key_point_layer.setZValue(3);

  this->setHandlesChildEvents(false);
  this->setAcceptHoverEvents(true);

  key_point_layer.setAcceptedMouseButtons(false);
  key_point_layer.setAcceptHoverEvents(false);
  key_point_layer.setAcceptTouchEvents(false);

  this->addToGroup(&trajectory_layer);
  this->addToGroup(&orientation_layer);
  this->addToGroup(&direction_layer);
  this->addToGroup(&key_point_layer);
}

void GraphicsTrajectoryItem::pushBackFrame(QPixmap img,
                                           QPointF center_coords_px,
                                           double angle,
                                           double meters_per_pixel,
                                           double quality)
{
  //setup map item
  //the frame_number is size
  auto frame_item = make_shared<GraphicsFrameItem>(img, frames.size());//, &trajectory_layer);

  makeTransforms(frame_item, frame_item->getMapItem().boundingRect().center(),
                 center_coords_px, angle, meters_per_pixel);

  QObject::connect( frame_item.get(), SIGNAL(stateChanged(int, bool)), this,
                                      SLOT(frameStateChanged(int, bool)) );

  frames.push_back(frame_item);
  trajectory_layer.addToGroup(frames.back().get());

  //setup orientation
  auto orient_item = make_shared<GraphicsOrientationItem>(QPointF(0, 0), 1);//, &orientation_layer);
  orient_item->setCenter(frame_item->getMapItem().boundingRect().center());
  orient_item->setAxisLength(frame_item->getMapItem().boundingRect().width()/4.0);

  makeTransforms(orient_item, frame_item->getMapItem().boundingRect().center(),
                 center_coords_px, angle, meters_per_pixel);

  orientations.push_back(orient_item);
  orientation_layer.addToGroup(orientations.back().get());

  //setup direction_layer
  double red_mul = quality < 0.5? 1.: 2*quality - 1;
  double green_mul = quality < 0.5? 2*quality: 1.;
  QColor way_point_color(int(255*red_mul), int(255*green_mul), 0);

  direction_layer.pushBackWayPoint((center_coords_px) * meters_per_pixel,
                                   way_point_color);
}

void GraphicsTrajectoryItem::addKeyPoint(int frame_num, QPointF center_px,
                                         double angle, double radius,
                                         QColor color)
{
  auto key_point = make_shared<GraphicsFastKeyPointItem>(QPointF(0, 0), 0, 1,
                                                         &key_point_layer);
  shared_ptr<GraphicsFrameItem> &frame_item = frames[frame_num];

  double scale = frame_item->transform().m11();
  QPointF item_center = center_px * scale;
  QPointF orig = QTransform().rotate(frame_item->rotation())
                             .map(frame_item->transformOriginPoint());
  QPointF frame_center = frame_item->pos() + orig*scale;

  key_point->setRadius(radius);
  key_point->setAngle(angle);
  key_point->setColor(color);

  key_point->setTransformOriginPoint((frame_center - item_center)/scale);
  key_point->setPos(item_center);
  key_point->setRotation(frame_item->rotation());
  key_point->setTransform(QTransform().translate(0, 0)
                                      .scale(scale, scale)
                                      .translate(0, 0));

  key_point->setAcceptHoverEvents(false);
  key_point->setAcceptedMouseButtons(false);
  key_point->setAcceptTouchEvents(false);

  key_points.push_back(key_point);
  key_point_layer.addToGroup(key_points.back().get());
}

void GraphicsTrajectoryItem::addKeyPointNew(QPointF pos, double angle,
                                            double radius, double scale,
                                            QColor color)
{
  auto key_point = make_shared<GraphicsFastKeyPointItem>(QPointF(0, 0), 0, 1,
                                                         &key_point_layer);

  key_point->setRadius(radius * scale);
  key_point->setAngle(angle);
  key_point->setColor(color);

  key_point->setPos(pos);
  key_points.push_back(key_point);
  key_point_layer.addToGroup(key_points.back().get());
}

void GraphicsTrajectoryItem::showFrame(int frame_num)
{
  frames[frame_num]->setVisible(true);
}

void GraphicsTrajectoryItem::hideFrame(int frame_num)
{
  frames[frame_num]->setVisible(false);
}

void GraphicsTrajectoryItem::makeTransforms(shared_ptr<QGraphicsItem> item,
                                            QPointF item_center_px,
                                            QPointF scene_center_pos_px,
                                            double angle,
                                            double meters_per_pixel)
{
  double scale = meters_per_pixel;
  //old transformations set
  //because of translate from qt4.6 is obsolete
  //item->setTransform(QTransform::fromTranslate(-item_center_px.x()*scale,
  //                                             -item_center_px.y()*scale),
  //                                             true);
  //item->setScale( meters_per_pixel / m_per_px );

  item->setTransformOriginPoint( item_center_px );
  item->setRotation(angle);
  qreal dx = 0;//item_center_px.x()/scale;
  qreal dy = 0;//item_center_px.y()/scale;
  item->setTransform(QTransform().translate(dx, dy)
                                 .scale(scale, scale)
                                 .translate(-dx, -dy), true);
  item->setPos( (scene_center_pos_px - item_center_px) * scale + this->pos());
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

void GraphicsTrajectoryItem::frameStateChanged(int frame_num, bool isSelected)
{
  //qDebug() << frame_num << " changed to " << isSelected;
  emit frameDoubleClicked(frame_num, isSelected);
}

void GraphicsTrajectoryItem::cleanSelection()
{
  for (auto &frame: frames)
  {
    frame->setSelected(false);
  }
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
