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

    ghostRecovery.setPen(QPen(QBrush(QColor(255, 255, 0)), 5));
    ghostRecovery.setBrush(QBrush(QColor(255, 255, 0), Qt::FDiagPattern));
    ghostOrientation.setAxisLength(0);

    main_map.setZValue(-1); 
    matches.setZValue(1);
    ghostRecovery.setZValue(2);
    ghostOrientation.setZValue(2.1);

    this->addItem(&trajectory1);
    this->addItem(&trajectory2);
    this->addItem(&main_map);
    matches.setOpacity(0.3);
    this->addItem(&matches);
    this->addItem(&ghostRecovery);
    this->addItem(&ghostOrientation);
}

void GraphicsMapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (main_view)
    {
        main_view->setMouseScenePosition(event->scenePos());
    }
    QGraphicsScene::mouseMoveEvent(event);
}

