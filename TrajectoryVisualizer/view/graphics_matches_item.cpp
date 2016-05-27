#include "graphics_matches_item.h"

#include <QLineF>

using namespace std;

GraphicsMatchesItem::GraphicsMatchesItem()
    : shift(0, 0)
{

}

void GraphicsMatchesItem::addLine(QPointF first_trj_pt, QPointF second_trj_pt)
{
    second_trj_pts.push_back(second_trj_pt);

    shared_ptr<QGraphicsLineItem> line = make_shared<QGraphicsLineItem>();

    line->setLine(QLineF(first_trj_pt, second_trj_pt + shift));

    lines.push_back(line);
    this->addToGroup(lines.back().get());
}

void GraphicsMatchesItem::setShift(double dx, double dy)
{
    setShift(QPointF(dx, dy));
}

void GraphicsMatchesItem::setShift(QPointF shift)
{
    this->shift = shift;

    for (int i = 0; i < lines.size(); i++)
    {
        auto &line = lines[i];

        line->setLine(QLineF(line->line().p1(), second_trj_pts[i] + shift));
    }
}


