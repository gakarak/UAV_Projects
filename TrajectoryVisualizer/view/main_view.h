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
    class MainView : public QMainWindow
    {
        Q_OBJECT

    public:
        /* constructors */
        explicit MainView(QWidget *parent = 0);
        ~MainView();

        /* main public interface */
        void setController(std::shared_ptr<controllerpkg::MainController> controller);

        void setFirstTrajectory(const std::vector<QPixmap> &imgs, const std::vector<QPointF> center_coords_px,
                                const std::vector<double> &angles, const std::vector<double> &meters_per_pixels,
                                const std::vector<double> &qualities);
        void setSecondTrajectory(const std::vector<QPixmap> &imgs, const std::vector<QPointF> center_coords_px,
                                 const std::vector<double> &angles, const std::vector<double> &meters_per_pixels,
                                 const std::vector<double> &qualities);
        void setFirstKeyPoints(const std::vector<int> &frames_num, const std::vector<QPointF> &centers_px, const std::vector<double> &angles,
                               const std::vector<double> &radius, const std::vector<QColor> &colors);
        void setSecondKeyPoints(const std::vector<int> &frames_num, const std::vector<QPointF> &centers_px, const std::vector<double> &angles,
                               const std::vector<double> &radius, const std::vector<QColor> &colors);

        void setMatches(const std::vector<std::vector<QPointF>> &trajectory_pts,
                        const std::vector<std::vector<QPointF>> &frames_center_on_map,
                        const std::vector<std::vector<double>> &angles,
                        const std::vector<std::vector<double>> &meters_per_pixels);

        void setGhostRecovery(QPointF center_px, QSize size, double angle, double meters_per_pixel, double coord_m_per_px);

        void setMainMap(QPixmap map, double meter_per_pixel);

        void setDetectors(const std::vector<QString> &detectors_names);
        void setDescriptors(const std::vector<QString> &descriptors_names);

        void showException(QString what);

        void setTrajectoryPath(int trj_num, QString path);

        GraphicsTrajectoryItem& getTrajectoryItem(int trj_num)
        {
            return trj_num == 0? scene.getFirstTrajectory():
                                 scene.getSecondTrajectory();
        }

    private slots:
        /* buttons */
        void on_load_ini_btn_clicked();
        void on_clear_btn_clicked();
        void on_calculate_btn_clicked();
        void on_match_btn_clicked();
        void on_recover_trajectory_btn_clicked();

        void on_load_first_trj_btn_clicked();
        void on_load_sec_trj_btn_clicked();

        /* checkboxes */
        void on_is_trajectory_show_chk_toggled(bool checked);
        void on_is_orientation_show_chk_toggled(bool checked);
        void on_is_direction_show_chk_toggled(bool checked);
        void on_is_key_point_show_chk_toggled(bool checked);
        void on_is_matches_show_check_toggled(bool checked);
        void on_is_map_show_check_toggled(bool checked);
        void on_is_recovery_show_check_toggled(bool checked);
        void on_filter_recovered_by_score_check_toggled(bool checked);

        /* frame selection */
        void onFirstTrajectoryDoubleClicked(int frame_num, bool isSelected);
        void onSecondTrajectoryDoubleClicked(int frame_num, bool isSelected);

    /* shift trajectory */
    private slots:
        void on_trj2_shift_x_spin_valueChanged(int shift_x);
        void on_trj2_shift_y_spin_valueChanged(int shift_y);
        void on_shift_second_trj_group_toggled(bool isChecked);

    private:
        void updateShift(int shift_x, int shift_y);
    /* shift trajectory */

    /* status bar */
    public:
        void setMouseScenePosition(QPointF pos);
        void setZoom(double zoom);

    private:
        void updateStatusBar();

        QPointF mouse_scene_pos_m;
        double  zoom;
    /* status bar */

    private://members
        std::shared_ptr<controllerpkg::MainController> controller;

        Ui::MainView *ui;
        GraphicsMapScene scene;
    };
}

#endif // MAIN_VIEW_H
