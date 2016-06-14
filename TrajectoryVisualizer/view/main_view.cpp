#include "main_view.h"
#include "ui_main_view.h"

#include <vector>
#include <QGraphicsItemGroup>
#include <QMessageBox>
#include <QDebug>
#include <QPen>
#include <QFileDialog>
#include <QIntValidator>

#include "controller/main_controller.h"
#include "config_singleton.h"

using namespace std;
using namespace viewpkg;

/*
 * constructors
 */

MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setMainView(this);
    ui->graphicsView->setMouseTracking(true);
    zoom = ui->graphicsView->getZoom();

    QObject::connect(ui->calculate_btn, SIGNAL(clicked(bool)), &scene.getFirstTrajectory(), SLOT(cleanSelection()));
    QObject::connect(ui->calculate_btn, SIGNAL(clicked(bool)), &scene.getSecondTrajectory(), SLOT(cleanSelection()));

    QObject::connect(&scene.getFirstTrajectory(), SIGNAL(frameDoubleClicked(int,bool)), this, SLOT(onFirstTrajectoryDoubleClicked(int,bool)));
    QObject::connect(&scene.getSecondTrajectory(), SIGNAL(frameDoubleClicked(int,bool)), this, SLOT(onSecondTrajectoryDoubleClicked(int,bool)));

    scene.getFirstTrajectory().setOrientationVisible(ui->is_orientation_show_chk->isChecked());
    scene.getSecondTrajectory().setOrientationVisible(ui->is_orientation_show_chk->isChecked());

    scene.getFirstTrajectory().setDirectionVisible(ui->is_direction_show_chk->isChecked());
    scene.getSecondTrajectory().setDirectionVisible(ui->is_direction_show_chk->isChecked());

    scene.getFirstTrajectory().setKeyPointsVisible(ui->is_key_point_show_chk->isChecked());
    scene.getSecondTrajectory().setKeyPointsVisible(ui->is_key_point_show_chk->isChecked());

    scene.getMainMap().setVisible(ui->is_map_show_check->isChecked());

    scene.getMatches().setVisible(ui->is_matches_show_check->isChecked());

    //intially update shift
    on_shift_second_trj_group_toggled(ui->shift_second_trj_group->isChecked());

    scene.setMainView(this);
}

MainView::~MainView()
{
    delete ui;
}

/*
 * main public interface
 */

void MainView::setController(shared_ptr<controllerpkg::MainController> controller)
{
    this->controller = controller;
}

void MainView::setFirstTrajectory(const vector<QPixmap> &imgs, const vector<QPointF> center_coords_px,
                                  const vector<double> &angles, const vector<double> &meters_per_pixels,
                                  const vector<double> &qualities)
{

    scene.getFirstTrajectory().clear();
    for (size_t i = 0; i < imgs.size(); i++)
    {
        scene.getFirstTrajectory().pushBackFrame(imgs[i], center_coords_px[i], angles[i], meters_per_pixels[i], qualities[i]);
    }
}

void MainView::setSecondTrajectory(const vector<QPixmap> &imgs, const vector<QPointF> center_coords_px,
                                   const vector<double> &angles, const vector<double> &meters_per_pixels,
                                   const vector<double> &qualities)
{
    scene.getSecondTrajectory().clear();
    for (size_t i = 0; i < imgs.size(); i++)
    {
        scene.getSecondTrajectory().pushBackFrame(imgs[i], center_coords_px[i], angles[i], meters_per_pixels[i], qualities[i]);
    }
}

void MainView::setFirstKeyPoints(const vector<int> &frames_num, const vector<QPointF> &centers_px, const vector<double> &angles,
                                 const vector<double> &radius, const vector<QColor> &colors)
{
    scene.getFirstTrajectory().clearKeyPoints();
    for (size_t i = 0; i < frames_num.size(); i++)
    {
        scene.getFirstTrajectory().addKeyPoint(frames_num[i], centers_px[i], angles[i], radius[i], colors[i]);
    }
}

void MainView::setSecondKeyPoints(const vector<int> &frames_num, const vector<QPointF> &centers_px, const vector<double> &angles,
                                 const vector<double> &radius, const vector<QColor> &colors)
{
    scene.getSecondTrajectory().clearKeyPoints();
    for (size_t i = 0; i < frames_num.size(); i++)
    {
        scene.getSecondTrajectory().addKeyPoint(frames_num[i], centers_px[i], angles[i], radius[i], colors[i]);
    }
}

void MainView::setMatches(const std::vector<std::vector<QPointF>> &trajectory_pts,
                          const std::vector<std::vector<QPointF>> &frames_center_on_map,
                          const std::vector<std::vector<double>> &angles,
                          const std::vector<std::vector<double>> &meters_per_pixels)
{
    scene.getMatches().clear();
    for (size_t i = 0; i < trajectory_pts.size(); i++)
    {
        scene.getMatches().addLine(trajectory_pts[i], frames_center_on_map[i], angles[i], meters_per_pixels[i]);
    }
}

void MainView::setGhostRecover(QPointF center_px, QSize size, double angle, double meters_per_pixel)
{
    double m_per_px = ConfigSingleton::getInstance().getCommonMetersPerPixel();
    double scale = meters_per_pixel / m_per_px;

    scene.getGhostRecover().setRect(0, 0, size.width(), size.height());
    scene.getGhostRecover().setTransformOriginPoint(QPointF(size.width(), size.height())/2.);
    scene.getGhostRecover().setPos((center_px - QPointF(size.width(), size.height())/2.)*scale);
    scene.getGhostRecover().setRotation(angle);
    qreal dx = 0;//item_center_px.x()/scale;
    qreal dy = 0;//item_center_px.y()/scale;
    scene.getGhostRecover().setTransform(QTransform().translate(dx, dy).scale(scale, scale)
                                                     .translate(-dx, -dy), false);
    qDebug() << "ghost recover setted" << endl;
}

void MainView::setMainMap(QPixmap map, double meter_per_pixel)
{
    double m_per_px = ConfigSingleton::getInstance().getCommonMetersPerPixel();

    scene.getMainMap().setMapItem(map);
    scene.getMainMap().setScale(  meter_per_pixel / m_per_px );
}

void MainView::setDetectors(const vector<QString> &detectors_names)
{
    ui->detector_combo->clear();
    for (const auto &name: detectors_names)
    {
        ui->detector_combo->addItem(name);
    }
}

void MainView::setDescriptors(const vector<QString> &descriptors_names)
{
    ui->descriptor_combo->clear();
    for (const auto &name: descriptors_names)
    {
        ui->descriptor_combo->addItem(name);
    }
}

void MainView::showException(QString what)
{
    QMessageBox::critical(this, "Error", what, QMessageBox::Ok, QMessageBox::Default);
}

/*
 * private slots
 * buttons
 */

void viewpkg::MainView::on_load_ini_btn_clicked()
{
    string ini_filename = ui->ini_edit->text().trimmed().toStdString();
    controller->loadIni(ini_filename);

    ui->model_settings_group->setEnabled(true);
}

void viewpkg::MainView::on_clear_btn_clicked()
{
    scene.getFirstTrajectory().clear();
    scene.getSecondTrajectory().clear();
    scene.getMatches().clear();

    ui->model_settings_group->setEnabled(false);
}

void viewpkg::MainView::on_calculate_btn_clicked()
{
    int detector_idx = ui->detector_combo->currentIndex();
    int descriptor_idx = ui->descriptor_combo->currentIndex();

    controller->loadOrCalculateModel(detector_idx, descriptor_idx);
}

void viewpkg::MainView::on_match_btn_clicked()
{
    int descriptor_idx = ui->descriptor_combo->currentIndex();

    controller->calculateMatches(descriptor_idx);
}

//obsolete
void MainView::on_load_btn_clicked()
{
    string trj1_filename = ui->first_traj_edit->text().trimmed().toStdString();
    string trj2_filename = ui->second_traj_edit->text().trimmed().toStdString();

    controller->loadMainMap("/home/pisarik/datasets/maps/my_set/yandex_roi_z16.png", 2.7958833);
    controller->loadTrajectories(trj1_filename, trj2_filename);

    ui->model_settings_group->setEnabled(true);
}

/*
 * checkboxes
 */

void viewpkg::MainView::on_is_trajectory_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setTrajectoryVisible(checked);
    scene.getSecondTrajectory().setTrajectoryVisible(checked);
}

void viewpkg::MainView::on_is_orientation_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setOrientationVisible(checked);
    scene.getSecondTrajectory().setOrientationVisible(checked);
}

void viewpkg::MainView::on_is_direction_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setDirectionVisible(checked);
    scene.getSecondTrajectory().setDirectionVisible(checked);
}

void viewpkg::MainView::on_is_key_point_show_chk_toggled(bool checked)
{
    scene.getFirstTrajectory().setKeyPointsVisible(checked);
    scene.getSecondTrajectory().setKeyPointsVisible(checked);
}

void viewpkg::MainView::on_is_matches_show_check_toggled(bool checked)
{
    scene.getMatches().setVisible(checked);
}

void viewpkg::MainView::on_is_map_show_check_toggled(bool checked)
{
    scene.getMainMap().setVisible(checked);
}

/*
 * frame selection
 */

void MainView::onFirstTrajectoryDoubleClicked(int frame_num, bool isSelected)
{
    if (isSelected)
    {
        scene.getSecondTrajectory().cleanSelection();
        controller->selectedFrame(0, frame_num);
    }
    else
    {
        controller->unselectedFrame(0, frame_num);
    }
}

void MainView::onSecondTrajectoryDoubleClicked(int frame_num, bool isSelected)
{
    if (isSelected)
    {
        scene.getFirstTrajectory().cleanSelection();
        controller->selectedFrame(1, frame_num);
    }
    else
    {
        controller->unselectedFrame(1, frame_num);
    }
}

/*
 * interface for trajectory shifting
 */
void viewpkg::MainView::on_trj2_shift_x_spin_valueChanged(int shift_x)
{
    updateShift(shift_x, ui->trj2_shift_y_spin->value());
}

void viewpkg::MainView::on_trj2_shift_y_spin_valueChanged(int shift_y)
{
    updateShift(ui->trj2_shift_x_spin->value(), shift_y);
}

void viewpkg::MainView::on_shift_second_trj_group_toggled(bool isChecked)
{
    if (isChecked)
    {
        updateShift(ui->trj2_shift_x_spin->value(),
                    ui->trj2_shift_y_spin->value());
    }
    else
    {
        updateShift(0, 0);
    }
}

void MainView::updateShift(int shift_x, int shift_y)
{
    QPointF shift(shift_x, shift_y);

    scene.getSecondTrajectory().setPos(shift);
    scene.getMatches().setShift(shift);
}

/*
 * interface for status bar
 */

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

void MainView::updateStatusBar()
{
    double m_per_px = ConfigSingleton::getInstance().getCommonMetersPerPixel();

    QString position_str = QString("Position: (%1, %2) meters").arg(QString::number(mouse_scene_pos_m.x(), 'f', 3), QString::number(mouse_scene_pos_m.y(), 'f', 3));
    QString zoom_str = QString("Zoom: %1%").arg(QString::number(zoom*100));
    QString meters_str = QString("Meters per pixel: %1").arg(QString::number(m_per_px / zoom));
    ui->statusBar->showMessage( QString("%3 | %2 | %1").arg(position_str, meters_str, zoom_str) );
}
