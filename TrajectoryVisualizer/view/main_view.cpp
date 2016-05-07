#include "main_view.h"
#include "ui_main_view.h"

#include <vector>
#include <QGraphicsItemGroup>
#include <QMessageBox>
#include <QDebug>
#include <QPen>

#include "controller/main_controller.h"
#include "config_singleton.h"

using namespace std;
using namespace viewpkg;

MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setMainView(this);
    ui->graphicsView->setMouseTracking(true);
    zoom = ui->graphicsView->getZoom();


    scene.getFirstTrajectory().setOrientationVisible(ui->is_orientation_show_chk->isChecked());
    scene.getSecondTrajectory().setOrientationVisible(ui->is_orientation_show_chk->isChecked());
    scene.getFirstTrajectory().setDirectionVisible(ui->is_direction_show_chk->isChecked());
    scene.getSecondTrajectory().setDirectionVisible(ui->is_direction_show_chk->isChecked());
    scene.getFirstTrajectory().setKeyPointsVisible(ui->is_key_point_show_chk->isChecked());
    scene.getSecondTrajectory().setKeyPointsVisible(ui->is_key_point_show_chk->isChecked());

    scene.getMainMap().setVisible(ui->is_map_show_check->isChecked());

    /*QGraphicsItem *r1 = new QGraphicsRectItem(0, 0, 50, 50);
    QGraphicsItem *r2 = new QGraphicsRectItem(0, 0, 50, 50);

    scene.addItem(r1);
    scene.addItem(r2);*/

    scene.setMainView(this);
}

MainView::~MainView()
{
    delete ui;
}

void MainView::setFirstTrajectory(const vector<QPixmap> &imgs, const vector<QPointF> center_coords_px,
                                  const vector<double> &angles, const vector<double> &meters_per_pixels)
{

    scene.getFirstTrajectory().clear();
    for (int i = 0; i < imgs.size(); i++)
    {
        scene.getFirstTrajectory().pushBackMap(imgs[i], center_coords_px[i], angles[i], meters_per_pixels[i]);
    }
}

void MainView::setSecondTrajectory(const vector<QPixmap> &imgs, const vector<QPointF> center_coords_px,
                                   const vector<double> &angles, const vector<double> &meters_per_pixels)
{
    scene.getSecondTrajectory().clear();
    for (int i = 0; i < imgs.size(); i++)
    {
        scene.getSecondTrajectory().pushBackMap(imgs[i], center_coords_px[i], angles[i], meters_per_pixels[i]);
    }
}

void MainView::setFirstKeyPoints(const vector<int> &maps_num, const vector<QPointF> &centers_px, const vector<double> &angles,
                                 const vector<double> &radius, const vector<QColor> &colors)
{
    scene.getFirstTrajectory().clearKeyPoints();
    for (int i = 0; i < maps_num.size(); i++)
    {
        scene.getFirstTrajectory().addKeyPoint(maps_num[i], centers_px[i], angles[i], radius[i], colors[i]);
    }
}

void MainView::setSecondKeyPoints(const vector<int> &maps_num, const vector<QPointF> &centers_px, const vector<double> &angles,
                                 const vector<double> &radius, const vector<QColor> &colors)
{
    scene.getSecondTrajectory().clearKeyPoints();
    for (int i = 0; i < maps_num.size(); i++)
    {
        scene.getSecondTrajectory().addKeyPoint(maps_num[i], centers_px[i], angles[i], radius[i], colors[i]);
    }
}

void MainView::setMainMap(QPixmap map, double meter_per_pixel)
{
    scene.getMainMap().setMapItem(map);
    scene.getMainMap().setScale(  meter_per_pixel / scene.getMetersPerPixel() );
}

void MainView::on_load_btn_clicked()
{
    string trj1_filename = ui->first_traj_edit->text().trimmed().toStdString();
    string trj2_filename = ui->second_traj_edit->text().trimmed().toStdString();

    controller->loadMainMap("/home/pisarik/datasets/maps/my_set/yandex_roi_z16.png", 2.7958833);
    controller->loadTrajectories(trj1_filename, trj2_filename);

    ui->calculate_btn->setEnabled(true);
}

void MainView::setController(shared_ptr<controllerpkg::MainController> controller)
{
    this->controller = controller;
}

void viewpkg::MainView::on_clear_btn_clicked()
{
    scene.getFirstTrajectory().clear();
    scene.getSecondTrajectory().clear();
}

void viewpkg::MainView::on_is_orientation_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setOrientationVisible(checked);
    scene.getSecondTrajectory().setOrientationVisible(checked);
}

void viewpkg::MainView::on_is_map_show_check_toggled(bool checked)
{
    scene.getMainMap().setVisible(checked);
}

void viewpkg::MainView::on_is_trajectory_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setTrajectoryVisible(checked);
    scene.getSecondTrajectory().setTrajectoryVisible(checked);
}

void MainView::updateStatusBar()
{
    QString position_str = QString("Position: (%1, %2) meters").arg(QString::number(mouse_scene_pos_m.x(), 'f', 3), QString::number(mouse_scene_pos_m.y(), 'f', 3));
    QString zoom_str = QString("Zoom: %1%").arg(QString::number(zoom*100));
    QString meters_str = QString("Meters per pixel: %1").arg(QString::number(scene.getMetersPerPixel() / zoom));
    ui->statusBar->showMessage( QString("%3 | %2 | %1").arg(position_str, meters_str, zoom_str) );
}

void MainView::setMouseScenePosition(QPointF pos)
{
    mouse_scene_pos_m = pos;
    updateStatusBar();
}

void MainView::setZoom(double zoom)
{
    this->zoom = zoom;
    updateStatusBar();
}

void viewpkg::MainView::on_is_direction_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setDirectionVisible(checked);
    scene.getSecondTrajectory().setDirectionVisible(checked);
}

void viewpkg::MainView::on_calculate_btn_clicked()
{
    controller->calculateKeyPoints();
}

void viewpkg::MainView::on_is_key_point_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setKeyPointsVisible(checked);
    scene.getSecondTrajectory().setKeyPointsVisible(checked);
}

void MainView::showException(QString what)
{
    QMessageBox::critical(this, "Error", what, QMessageBox::Ok, QMessageBox::Default);
}
void viewpkg::MainView::on_load_ini_btn_clicked()
{
    string ini_filename = ui->ini_edit->text().trimmed().toStdString();
    controller->loadIni(ini_filename);

    ui->calculate_btn->setEnabled(true);
}
