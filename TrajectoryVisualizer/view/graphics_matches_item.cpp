#include "graphics_matches_item.h"

#include <vector>

#include <QLineF>
#include <QPen>
#include <QBrush>

#include "config_singleton.h"

using namespace std;

GraphicsMatchesItem::GraphicsMatchesItem()
    : shift(0, 0)
{

}

void GraphicsMatchesItem::addLine(vector<QPointF> trajectories_pt, vector<QPointF> frames_center_on_map,
                                  vector<double> angles, vector<double> meters_per_pixels)
{
    for (int i = 0; i < 2; i++)
    {
        double scale = meters_per_pixels[i];

        trajectories_pt[i] *= scale;

        frames_center_on_map[i] *= scale;
        trajectories_pt[i] -= frames_center_on_map[i];
        trajectories_pt[i] = QTransform().rotate(angles[i]).map(trajectories_pt[i]);
        trajectories_pt[i] += frames_center_on_map[i];
    }

    second_trj_pts.push_back(trajectories_pt[1]);

    shared_ptr<QGraphicsLineItem> line = make_shared<QGraphicsLineItem>(this);

    line->setPen(QPen(QBrush(QColor(51, 102, 153)), 0.5));

    line->setLine(QLineF(trajectories_pt[0], trajectories_pt[1] + shift));
    line->setAcceptedMouseButtons(false);

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

    for (size_t i = 0; i < lines.size(); i++)
    {
        auto &line = lines[i];

        line->setLine(QLineF(line->line().p1(), second_trj_pts[i] + shift));
    }
}

void GraphicsMatchesItem::clear()
{
    second_trj_pts.clear();
    lines.clear();
}


