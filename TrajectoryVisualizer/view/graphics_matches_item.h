#ifndef GRAPHICSMATCHESITEM_H
#define GRAPHICSMATCHESITEM_H

#include <vector>
#include <memory>

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>

class GraphicsMatchesItem : public QGraphicsItemGroup
{
public:
    GraphicsMatchesItem();

    //void addLine(QPointF first_trj_pt, QPointF second_trj_pt, double meters_per_pixel);
    void addLine(std::vector<QPointF> trajectories_pt, std::vector<QPointF> frames_center_on_map,
                 std::vector<double> angles, std::vector<double> meters_per_pixels);

    void setShift(double dx, double dy);
    void setShift(QPointF shift);

    void clear();

private:
    std::vector<std::shared_ptr<QGraphicsLineItem>> lines;

    std::vector<QPointF> second_trj_pts;
    QPointF shift;
};

#endif // GRAPHICSMATCHESITEM_H
