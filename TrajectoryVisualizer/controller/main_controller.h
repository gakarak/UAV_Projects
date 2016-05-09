#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <memory>
#include <string>
#include <vector>

#include "view/main_view.h"
#include "model/main_model.h"

namespace controllerpkg
{

    class MainController
    {
    public:

        void calculateKeyPoints();

        void loadIni(std::string ini_filename);
        void loadTrajectories(std::string trj1_filename, std::string trj2_filename);
        void loadMainMap(std::string filename, double meters_per_pixel);

        void loadKeyPoints(std::string filename, int trj_num);
        void saveKeyPoints(std::string filename, int trj_num);

        void showFirstTrajectory();
        void showSecondTrajectory();
        void showMainMap();
        void showFirstKeyPoints();
        void showSecondKeyPoints();

        void setView(std::shared_ptr<viewpkg::MainView> view)       { this->view = view; }
        void setModel(std::shared_ptr<modelpkg::MainModel> model)   { this->model = model; }

        void showException(std::string what);

    private:
        void                    calculateMapsQuality();

        //params - filename, x_m, y_m, angle, m_per_px
        modelpkg::Map           loadMapFromRow(std::vector<std::string> params);
        modelpkg::Trajectory    loadTrjFromCsv(std::string csv_filename);

    private:
        std::shared_ptr<viewpkg::MainView> view;
        std::shared_ptr<modelpkg::MainModel> model;
    };

}

#endif //MAIN_CONTROLLER_H
