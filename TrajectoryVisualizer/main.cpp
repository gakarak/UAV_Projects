#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

#include <QApplication>

#include "config_singleton.h"
#include "model/main_model.h"
#include "controller/main_controller.h"
#include "view/main_view.h"
#include "algorithms/transformator.h"
#include "algorithms/trajectory_loader.h"
#include "algorithms/image_info_gradient_estimator.h"
#include "utils/geom_utils.h"
#include "algorithms/restorer_by_cloud.h"

using namespace std;
using namespace algorithmspkg;

int main(int argc, char *argv[])
{
    //for commit
    QApplication a(argc, argv);

    shared_ptr<modelpkg::MainModel> main_model = make_shared<modelpkg::MainModel>();
    shared_ptr<controllerpkg::MainController> main_controller = make_shared<controllerpkg::MainController>();
    shared_ptr<viewpkg::MainView> main_view = make_shared<viewpkg::MainView>();

    main_controller->setModel(main_model);
    main_controller->setView(main_view);

    main_view->setController(main_controller);

    if (argc == 1)
    {
        main_controller->showView();
        return a.exec();
    }
    else
    {
        if (argc != 6 && argc != 7)
        {
            cout << "Using: " << endl;
            cout << "       TrajectoryVisualizer train_trj_path que_trj_path detector descriptor csv_path [a]" << endl;
            cout << "       detector - SIFT SURF KAZE AKAZE BRISK" << endl;
            cout << "       descriptor - SIFT SURT KAZE AKAZE BRISK FREAK" << endl;
            cout << "       a - stands for append" << endl;
            return 1;
        }

        const auto &detectors_names = main_controller->getDetectorsNames();
        const auto &descr_names = main_controller->getDescriptorsNames();

        auto it = find(detectors_names.begin(), detectors_names.end(), argv[3]);
        if (it == detectors_names.end())
        {
            cout << "Error: detector not found" << endl;
            return 2;
        }
        int detector_idx = it - detectors_names.begin();
        auto detector = main_controller->getDetector(detector_idx);

        it = find(descr_names.begin(), descr_names.end(), argv[4]);
        if (it == descr_names.end())
        {
            cout << "Error: descrpiptor not found" << endl;
            return 3;
        }
        int descriptor_idx = it - descr_names.begin();
        auto descriptor = main_controller->getDescriptor(descriptor_idx);

        main_model->setTrajectory(0, TrajectoryLoader::loadTrajectory(argv[1]));
        main_model->setTrajectory(1, TrajectoryLoader::loadTrajectory(argv[2]));
        //load or calculate keypoints
        {
            auto &trj = main_model->getTrajectory(0);

            string path_to_kp_bin = ConfigSingleton::getPathToKeyPoints(argv[1], detectors_names[detector_idx].toStdString());

            try
            {
                TrajectoryLoader::loadKeyPoints(trj, path_to_kp_bin);
            }
            catch (TrajectoryLoader::NoFileExist &e)
            {
                clog << e.what() << endl;
                clog << "Calculating key points for train trajectory\n";
                TrajectoryLoader::calculateKeyPoints(trj, detector);

                clog << "Saving key points" << endl;
                TrajectoryLoader::saveKeyPoints(trj, path_to_kp_bin);
            }
        }
        //load or calculate descriptions
        {
            auto &trj = main_model->getTrajectory(0);
            string path_to_dscr_xml = ConfigSingleton::getPathToDescriptors(argv[1], detectors_names[detector_idx].toStdString(),
                                                                        descr_names[descriptor_idx].toStdString());
            try
            {
                TrajectoryLoader::loadDescriptions(trj, path_to_dscr_xml);
            }
            catch (TrajectoryLoader::NoFileExist &e)
            {
                clog << e.what() << endl;
                clog << "Calculating descriptors for train trajectory\n";
                TrajectoryLoader::calculateDescriptions(trj, descriptor);

                clog << "Saving descriptors" << endl;
                TrajectoryLoader::saveDescriptions(trj, path_to_dscr_xml);
            }
        }

        shared_ptr<FeatureBasedRestorer> restorer(
                              new RestorerByCloud(detector, descriptor));

        {//prepare recover
            auto &trj = main_model->getTrajectory(0);
            for (size_t frame_num = 0; frame_num < trj.getFramesCount(); frame_num++)
            {
                restorer->addFrame(trj.getFrame(frame_num).image_center,
                                 trj.getFrameAllKeyPoints(frame_num),
                                 trj.getFrameDescription(frame_num),
                                 trj.getFrame(frame_num).pos_m,
                                 -trj.getFrame(frame_num).angle,
                                 trj.getFrame(frame_num).m_per_px);
            }
        }

        ofstream out;
        if (argc == 6)
        {
            out.open(argv[5]);
            out << "Quality,euclid_distance_btw_centers,angle_difference_degree\n";
        }
        else if (string(argv[6]) == "a") //apend
        {
            out.open(argv[5], std::fstream::app);
        }
        else
        {
            out.open(argv[5]);
            out << "Quality,euclid_distance_btw_centers,angle_difference_degree\n";
        }

        //start evaluating
        size_t skipped = 0;
        auto &que_trj = main_model->getTrajectory(1);
        for (size_t frame_num = 0; frame_num < que_trj.getFramesCount(); frame_num++)
        {
            const auto& frame = que_trj.getFrame(frame_num);

            cv::Point2f pos;
            double angle;
            double scale;
            restorer->recoverLocation(frame.image,
                                      pos, angle, scale);

            double quality = ImageInfoGradientEstimator(frame.m_per_px / 4.).estimate(frame.image);
            if (!restorer->getLastHomography().empty())
            {
                out << quality << ',' <<
                       cv::norm(pos - frame.pos_m) << ',' <<
                       fabs(angle - frame.angle) << endl;
            }
            else
            {
                skipped++;
            }
        }
        cout << "Skipped: " << skipped << endl;

        return 0;
    }
}
