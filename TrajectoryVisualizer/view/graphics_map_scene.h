#ifndef GRAPHICSMAPSCENE_H
#define GRAPHICSMAPSCENE_H

#include <memory>

#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include "graphics_map_item.h"
#include "graphics_trajectory_item.h"
#include "graphics_matches_item.h"

namespace viewpkg
{
    class MainView;

    class GraphicsMapScene : public QGraphicsScene
    {
    public:
        GraphicsMapScene();

        void    setMainView(MainView *main_view)   { this->main_view = main_view;   }


        GraphicsTrajectoryItem&     getFirstTrajectory()    { return trajectory1; }
        GraphicsTrajectoryItem&     getSecondTrajectory()   { return trajectory2; }
        GraphicsMapItem&            getMainMap()            { return main_map; }
        GraphicsMatchesItem&        getMatches()            { return matches; }
        QGraphicsRectItem&          getGhostRecovery()      { return ghostRecovery; }

        const GraphicsTrajectoryItem&     getFirstTrajectory() const    { return trajectory1; }
        const GraphicsTrajectoryItem&     getSecondTrajectory() const   { return trajectory2; }
        const GraphicsMapItem&            getMainMap() const            { return main_map; }
        const GraphicsMatchesItem&        getMatches() const            { return matches; }
        const QGraphicsRectItem&          getGhostRecovery() const      { return ghostRecovery; }

        //double                            getMetersPerPixel() const     { return m_per_px; }
    protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    private:
        MainView   *main_view;

        GraphicsTrajectoryItem trajectory1;
        GraphicsTrajectoryItem trajectory2;

        GraphicsMatchesItem matches;

        QGraphicsRectItem ghostRecovery;

        GraphicsMapItem main_map;

        //double m_per_px;
    };

}

#endif // GRAPHICSMAPSCENE_H
