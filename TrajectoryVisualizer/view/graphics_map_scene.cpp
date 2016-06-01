#include "graphics_map_scene.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include "main_view.h"
#include "config_singleton.h"

using namespace viewpkg;

GraphicsMapScene::GraphicsMapScene()
    :  main_view(nullptr)
{
    //trajectory1.setOrientationVisible(ui->is_orientation_show_chk->isChecked());
    //trajectory2.setOrientationVisible(ui->is_orientation_show_chk->isChecked());

    main_map.setZValue(-1);
    //main_map.setVisible(ui->is_map_show_check->isChecked());
    main_map.setHoverable(false);

    matches.setZValue(1);

    this->addItem(&trajectory1);
    this->addItem(&trajectory2);
    this->addItem(&main_map);
    matches.setOpacity(0.3);
    this->addItem(&matches);
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

