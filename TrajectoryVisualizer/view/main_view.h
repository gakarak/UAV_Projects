#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include <memory>
#include <vector>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QColor>

#include "graphics_map_scene.h"
#include "graphics_map_view.h"

//#include "controller/main_controller.h"
namespace controllerpkg
{
    class MainController;
}

namespace Ui {
    class MainView;
}

namespace viewpkg
{
    //need refactor... trajectories and maps must be moved into scene
    //also question abouts spread m_per_px param
    class MainView : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainView(QWidget *parent = 0);
        ~MainView();

        void setController(std::shared_ptr<controllerpkg::MainController> controller);

        void setFirstTrajectory(const std::vector<QPixmap> &imgs, const std::vector<QPointF> center_coords_px,
                                const std::vector<double> &angles, const std::vector<double> &meters_per_pixels,
                                const std::vector<double> &qualities);
        void setSecondTrajectory(const std::vector<QPixmap> &imgs, const std::vector<QPointF> center_coords_px,
                                 const std::vector<double> &angles, const std::vector<double> &meters_per_pixels,
                                 const std::vector<double> &qualities);
        void setFirstKeyPoints(const std::vector<int> &maps_num, const std::vector<QPointF> &centers_px, const std::vector<double> &angles,
                               const std::vector<double> &radius, const std::vector<QColor> &colors);
        void setSecondKeyPoints(const std::vector<int> &maps_num, const std::vector<QPointF> &centers_px, const std::vector<double> &angles,
                               const std::vector<double> &radius, const std::vector<QColor> &colors);

        void setMainMap(QPixmap map, double meter_per_pixel);

        //for statusBar
        void setMouseScenePosition(QPointF pos);
        void setZoom(double zoom);

        void showException(QString what);

    private slots:
        void on_load_btn_clicked();
        void on_clear_btn_clicked();

        void on_is_orientation_show_chk_toggled(bool checked);
        void on_is_map_show_check_toggled(bool checked);
        void on_is_trajectory_show_chk_toggled(bool checked);

        void on_is_direction_show_chk_toggled(bool checked);

        void on_calculate_btn_clicked();

        void on_is_key_point_show_chk_toggled(bool checked);

        void on_load_ini_btn_clicked();

        void on_save_kp_btn_clicked();

    private://for statusBar
        void updateStatusBar();
        QPointF mouse_scene_pos_m;
        double  zoom;

    private://members
        std::shared_ptr<controllerpkg::MainController> controller;

        Ui::MainView *ui;
        GraphicsMapScene scene;

        /*GraphicsTrajectoryItem trajectory1;
        GraphicsTrajectoryItem trajectory2;

        GraphicsMapItem main_map;*/
    };
}

#endif // MAIN_VIEW_H
