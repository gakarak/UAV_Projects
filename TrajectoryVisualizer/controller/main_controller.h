#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <QString>

#include <memory>
#include <string>
#include <vector>

#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include "view/main_view.h"
#include "model/main_model.h"

namespace controllerpkg
{

    class MainController
    {
    public:
        MainController();

        void loadOrCalculateModel(int detector_idx, int descriptor_idx);

        void loadIni(std::string ini_filename);
        void loadTrajectories(std::string trj1_filename, std::string trj2_filename);
        void loadMainMap(std::string filename, double meters_per_pixel);

        void showFirstTrajectory();
        void showSecondTrajectory();
        void showMainMap();
        void showFirstKeyPoints();
        void showSecondKeyPoints();

        void setView(std::shared_ptr<viewpkg::MainView> view)       { this->view = view; }
        void setModel(std::shared_ptr<modelpkg::MainModel> model)   { this->model = model; }

        void showException(std::string what);
        void showView();

        //exceptions
        class Exception: public std::runtime_error
        {
        public:
            Exception(const std::string &what): std::runtime_error("MainController: " + what)
            {}
        };

        class NoFileExist: public Exception
        {
        public:
            NoFileExist(const std::string filename): Exception("Cannot open file: " + filename)
            {}
        };

    private:
        void initDetectors();
        void initDescriptors();

        void calculateMapsQuality();

        void loadOrCalculateKeyPoints(int detector_idx);
        void       calculateKeyPoints(int trj_num, int detector_idx);
        void            loadKeyPoints(int trj_num, std::string filename);
        void            saveKeyPoints(int trj_num, std::string filename);

        void loadOrCalculateDescriptions(int detector_idx, int descriptor_idx);
        void       calculateDescriptions(int trj_num, int descriptor_idx);
        void            loadDescriptions(int trj_num, std::string filename);
        void            saveDescriptions(int trj_num, std::string filename);

                                //params - filename, x_m, y_m, angle, m_per_px
        modelpkg::Map           loadMapFromRow(std::vector<std::string> params);
        modelpkg::Trajectory    loadTrjFromCsv(std::string csv_filename);

    private:
        std::shared_ptr<viewpkg::MainView> view;
        std::shared_ptr<modelpkg::MainModel> model;

        std::vector<QString> detectors_names;
        std::vector<cv::Ptr<cv::Feature2D>> detectors;

        std::vector<QString> descriptors_names;
        std::vector<cv::Ptr<cv::Feature2D>> descriptors;
        std::vector<cv::NormTypes> norm_types;
    };

}

#endif //MAIN_CONTROLLER_H
