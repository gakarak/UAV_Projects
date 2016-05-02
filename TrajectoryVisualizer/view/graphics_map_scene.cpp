#include "graphics_map_scene.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include "main_view.h"

using namespace viewpkg;

GraphicsMapScene::GraphicsMapScene(double m_per_px)
    :  m_per_px(m_per_px), main_view(nullptr)
{
    //trajectory1.setOrientationVisible(ui->is_orientation_show_chk->isChecked());
    //trajectory2.setOrientationVisible(ui->is_orientation_show_chk->isChecked());

    main_map.setZValue(-1);
    //main_map.setVisible(ui->is_map_show_check->isChecked());
    main_map.setHoverable(false);

    this->addItem(&trajectory1);
    this->addItem(&trajectory2);
    this->addItem(&main_map);
}

void GraphicsMapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (main_view)
    {
        main_view->setMouseScenePosition(event->scenePos() * m_per_px);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

