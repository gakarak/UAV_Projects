#include "graphics_map_scene.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>

#include "main_view.h"
#include "config_singleton.h"

using namespace viewpkg;

GraphicsMapScene::GraphicsMapScene()
    :  main_view(nullptr)
{
    main_map.setHoverable(false);

    matches.setFiltersChildEvents(true);

    ghostRecover.setPen(QPen(QBrush(QColor(255, 255, 0)), 3));

    main_map.setZValue(-1); 
    matches.setZValue(1);
    ghostRecover.setZValue(2);

    this->addItem(&trajectory1);
    this->addItem(&trajectory2);
    this->addItem(&main_map);
    matches.setOpacity(0.3);
    this->addItem(&matches);
    this->addItem(&ghostRecover);
}

void GraphicsMapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    double m_per_px = ConfigSingleton::getInstance().getCommonMetersPerPixel();

    if (main_view)
    {
        main_view->setMouseScenePosition(event->scenePos() * m_per_px);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

