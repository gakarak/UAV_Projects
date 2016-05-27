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

    void addLine(QPointF first_trj_pt, QPointF second_trj_pt);

    void setShift(double dx, double dy);
    void setShift(QPointF shift);

private:
    std::vector<std::shared_ptr<QGraphicsLineItem>> lines;

    std::vector<QPointF> second_trj_pts;
    QPointF shift;
};

#endif // GRAPHICSMATCHESITEM_H
