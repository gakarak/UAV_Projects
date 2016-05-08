#include "graphics_direction_item.h"

#include <QPen>
#include <QBrush>

using namespace std;
using namespace viewpkg;

GraphicsDirectionItem::GraphicsDirectionItem()
{

}

void GraphicsDirectionItem::pushBackWayPoint(QPointF scene_center_pos_px, QColor brush_color)
{
    //because of translate from qt4.6 is obsolete
    //item->setTransform(QTransform::fromTranslate(-item_center_px.x(), -item_center_px.y()), true);
    //QGraphicsEllipseItem i();
    //i.setBrush(QBrush(color));
    points.push_back(make_shared<QGraphicsEllipseItem>(-5, -5, 10, 10));
    points.back()->setPen(QPen(QBrush(QColor(0, 0, 255)), 2, Qt::SolidLine));
    points.back()->setBrush(QBrush(brush_color));

    points.back()->setZValue(1);

    points.back()->setTransformOriginPoint( points.back()->boundingRect().center() );
    //points.back()->setScale( meters_per_pixel / m_per_px );
    points.back()->setPos( scene_center_pos_px );// / (m_per_px / meters_per_pixel) );

    if (points.size() > 1)
    {
        edges.push_back(make_shared<QGraphicsLineItem>(points.back()->pos().x(), points.back()->pos().y(),
                                                       points.end()[-2]->pos().x(), points.end()[-2]->pos().y()
                                                       ));
        edges.back()->setPen(QPen(QBrush(QColor(0, 0, 255)), 2, Qt::SolidLine));
        edges.back()->setZValue(0);

        this->addToGroup(edges.back().get());
    }

    this->addToGroup(points.back().get());
}

void GraphicsDirectionItem::clear()
{
    points.clear();
    edges.clear();
}
