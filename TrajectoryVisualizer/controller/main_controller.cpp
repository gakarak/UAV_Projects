#include "main_controller.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include <QPixmap>
#include <QPointF>
#include <QDebug>

#include <opencv2/features2d.hpp>

#include "utils/acmopencv.h"
#include "utils/csv.h"
#include "utils/gradient_density.h"
#include "config_singleton.h"

using namespace std;
using namespace modelpkg;
using namespace controllerpkg;

MainController::MainController()
{
    initDetectors();
    initDescriptors();

    trajectories_kp_cloud.assign(2, vector<cv::KeyPoint>());
    trajectories_selected_frames.assign(2, vector<int>());
}

void MainController::loadOrCalculateModel(int detector_idx, int descriptor_idx)
{
    loadOrCalculateKeyPoints(detector_idx);
    loadOrCalculateDescriptions(detector_idx, descriptor_idx);

    for (int trj_num = 0; trj_num < model->getTrajectoriesCount(); trj_num++)
    {
        this->showKeyPoints(trj_num);
    }
}

void MainController::loadIni(string ini_filename)
{
    clear();

    ConfigSingleton &cfg = ConfigSingleton::getInstance();
    try
    {
        cfg.loadIni(ini_filename);
        loadTrajectories(cfg.getPathToFirstTrajectoryCsv(),
                         cfg.getPathToSecondTrajectoryCsv());

        loadMainMap(cfg.getPathToMapCsv(),
                    cfg.getMapMetersPerPixel());
    }
    catch (ConfigSingleton::Exception &e)
    {
        this->showException(e.what());
    }
}

void MainController::loadTrajectories(string trj1_filename, string trj2_filename)
{
    model->setTrajectory(0, loadTrjFromCsv(trj1_filename));
    model->setTrajectory(1, loadTrjFromCsv(trj2_filename));

    this->calculateFramesQuality();

    for (int trj_num = 0; trj_num < model->getTrajectoriesCount(); trj_num++)
    {
        this->showTrajectory(trj_num);
    }
}

void MainController::loadMainMap(string filename, double meters_per_pixel)
{
    model->getMainMap() = modelpkg::Map(filename, 0, 0, 0, meters_per_pixel);

    double center_x_m = model->getMainMap().image.cols / 2.0 * meters_per_pixel;
    double center_y_m = model->getMainMap().image.rows / 2.0 * meters_per_pixel;
    model->getMainMap().pos_m = cv::Point2f(center_x_m, center_y_m);

    this->showMainMap();
}

void MainController::showTrajectory(int trj_num)
{
    vector<QPixmap> qpixs;
    vector<QPointF> center_coords_px;
    vector<double>  angles;
    vector<double>  meter_per_pixels;
    const vector<double> &qualities = model->getTrajectory(trj_num).frames_quality;

    for (const auto &frame: model->getTrajectory(trj_num).frames)
    {
        QPointF center_px;
        center_px.setX( frame.pos_m.x / frame.m_per_px );
        center_px.setY( frame.pos_m.y / frame.m_per_px );

        qpixs.push_back( utils::ASM::cvMatToQPixmap(frame.image) );
        center_coords_px.push_back( center_px );
        angles.push_back( frame.angle );
        meter_per_pixels.push_back( frame.m_per_px );
    }
    if (trj_num == 0)
    {
        view->setFirstTrajectory(qpixs, center_coords_px, angles, meter_per_pixels, qualities);
    }
    else
    {
        view->setSecondTrajectory(qpixs, center_coords_px, angles, meter_per_pixels, qualities);
    }
}

void MainController::showMainMap()
{
    QPixmap qpix = utils::ASM::cvMatToQPixmap(model->getMainMap().image);

    view->setMainMap(qpix, model->getMainMap().m_per_px);
}

void MainController::showKeyPoints(int trj_num)
{
    const Trajectory &trj = model->getTrajectory(trj_num);
    const auto &key_points = trj.key_points;

    vector<int> frames_num;
    vector<QPointF> center_coords_px;
    vector<double> angles;
    vector<double> radius;
    vector<QColor> colors;

    for (int i = 0; i < key_points.size(); i+=1)
    {
        for (int j = 0; j < std::min((size_t)100, key_points[i].size()); j++)
        {
            const Map &frame = trj.frames[i];
            const cv::KeyPoint &kp = key_points[i][j];

            double mul = kp.response / 100;
            if (mul > 1) mul = 1;

            frames_num.push_back(i);
            center_coords_px.push_back(QPointF(kp.pt.x, kp.pt.y) - QPointF(frame.image.cols/2, frame.image.rows/2) + QPointF(frame.pos_m.x / frame.m_per_px, frame.pos_m.y / frame.m_per_px));
            angles.push_back(kp.angle);
            radius.push_back(kp.size / 2.);
            colors.push_back( QColor(255*(1-mul), 255*(mul), 0) );//QColor(255*mul, 165*mul, 0));
        }
    }

    if (trj_num == 0)
    {
        view->setFirstKeyPoints(frames_num, center_coords_px, angles, radius, colors);
    }
    else
    {
        view->setSecondKeyPoints(frames_num, center_coords_px, angles, radius, colors);
    }
}

void MainController::selectedFrame(int trj_num, int frame_num)
{
    trajectories_selected_frames[trj_num].push_back(frame_num);

    updateTrajectoryKeyPointsCloud(trj_num);
}

void MainController::unselectedFrame(int trj_num, int frame_num)
{
    auto &selected_frames = trajectories_selected_frames[trj_num];
    auto where = find(selected_frames.begin(), selected_frames.end(), frame_num);
    selected_frames.erase(where);

    updateTrajectoryKeyPointsCloud(trj_num);
}

void MainController::showException(string what)
{
    clog << "Exception! " << what << endl;
    view->showException(QString::fromStdString(what));
}

void MainController::showView()
{
    view->setDetectors(detectors_names);
    view->setDescriptors(descriptors_names);
    view->show();
}

/*
 * PRIVATE SECTION
 */

void MainController::calculateFramesQuality()
{
    double threshold = ConfigSingleton::getInstance().getQualityThreshold();

    for (int trj_num = 0; trj_num < model->getTrajectoriesCount(); trj_num++)
    {
        Trajectory &trj = model->getTrajectory(trj_num);
        const auto& frames = trj.frames;

        for (const auto& frame: frames)
        {
            double scale = frame.m_per_px / ConfigSingleton::getInstance().getGradientMetersPerPixel();
            cv::Size new_size(frame.image.size().width * scale, frame.image.size().height * scale);

            cv::Mat resized;
            cv::resize(frame.image, resized, new_size);

            double quality = std::min( 1., utils::cv::gradientDensity(resized) / threshold );

            trj.frames_quality.push_back(quality);
        }
    }

}

/*
 * Key points
 */
void MainController::loadOrCalculateKeyPoints(int detector_idx)
{
    ConfigSingleton &cfg = ConfigSingleton::getInstance();

    for (int trj_num = 0; trj_num < model->getTrajectoriesCount(); trj_num++)
    {
        string path_to_kp_bin = cfg.getPathToKeyPoints(trj_num, detectors_names[detector_idx].toStdString());

        try
        {
            loadKeyPoints(trj_num, path_to_kp_bin);
        }
        catch (MainController::NoFileExist &e)
        {
            clog << e.what() << endl;
            clog << "Calculating key points for trajectory #" << trj_num+1 << endl;
            calculateKeyPoints(trj_num, detector_idx);

            clog << "Saving key points" << endl;
            saveKeyPoints(trj_num, path_to_kp_bin);
        }

        //making cloud of key_points
        /*Trajectory &trj = model->getTrajectory(trj_num);
        for (const auto& frame_key_points: trj.key_points)
        {
            trj.cloud.insert(trj.cloud.end(), frame_key_points.begin(), frame_key_points.end());
        }*/
    }
}

void MainController::calculateKeyPoints(int trj_num, int detector_idx)
{
    cv::Ptr<cv::Feature2D> &detector = detectors[detector_idx];
    Trajectory &trj = model->getTrajectory(trj_num);

    trj.key_points.clear();
    for (const Map& frame: trj.frames)
    {
        trj.key_points.push_back(vector<cv::KeyPoint>());
        detector->detect(frame.image, trj.key_points.back());
    }

    for (auto &frame_kp: trj.key_points)
    {
        std::sort(frame_kp.begin(), frame_kp.end(), []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
                                         bool { return left.response > right.response; });
    }
}

void MainController::loadKeyPoints(int trj_num, string filename)
{
    ifstream in(filename, ios::binary);
    if (!in)
    {
        throw MainController::NoFileExist(filename);
    }

    auto &key_points = model->getTrajectory(trj_num).key_points;
    key_points.clear();

    //maybe format checking
    size_t n = 0;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    for (int i = 0; i < n; i++)
    {
        key_points.push_back(vector<cv::KeyPoint>());

        size_t m = 0;
        in.read(reinterpret_cast<char*>(&m), sizeof(m));
        for (int j = 0; j < m; j++)
        {
            cv::KeyPoint kp;
            in.read(reinterpret_cast<char*>(&kp), sizeof(kp));
            key_points[i].push_back(kp);
        }
    }
}

void MainController::saveKeyPoints(int trj_num, string filename)
{
    const auto &key_points = model->getTrajectory(trj_num).key_points;

    ofstream out(filename, ios::binary);
    size_t n = key_points.size();
    out.write(reinterpret_cast<char*>(&n), sizeof(n));
    for (int i = 0; i < key_points.size(); i++)
    {
        size_t m = key_points[i].size();
        out.write(reinterpret_cast<char*>(&m), sizeof(m));
        for (int j = 0; j < key_points[i].size(); j++)
        {
            out.write(reinterpret_cast<const char*>(&key_points[i][j]), sizeof(key_points[i][j]));
        }
    }
}

/*
 * Descriptions
 */

void MainController::loadOrCalculateDescriptions(int detector_idx, int descriptor_idx)
{
    ConfigSingleton &cfg = ConfigSingleton::getInstance();

    for (int trj_num = 0; trj_num < model->getTrajectoriesCount(); trj_num++)
    {
        string path_to_dscr_xml = cfg.getPathToDescriptors(trj_num, detectors_names[detector_idx].toStdString(),
                                                                    descriptors_names[descriptor_idx].toStdString());

        try
        {
            loadDescriptions(trj_num, path_to_dscr_xml);
        }
        catch (MainController::NoFileExist &e)
        {
            clog << e.what() << endl;
            clog << "Calculating descriptors for trajectory #" << trj_num+1 << endl;
            calculateDescriptions(trj_num, detector_idx);

            clog << "Saving descriptors" << endl;
            saveDescriptions(trj_num, path_to_dscr_xml);
        }
    }

}

void MainController::calculateDescriptions(int trj_num, int descriptor_idx)
{
    cv::Ptr<cv::Feature2D> &descriptor = descriptors[descriptor_idx];
    Trajectory &trj = model->getTrajectory(trj_num);

    trj.descriptions.clear();
    for (int i = 0; i < trj.key_points.size(); i++)
    {
        cv::Mat descr;
        descriptor->compute(trj.frames[i].image, trj.key_points[i], descr);

        trj.descriptions.push_back(descr);
    }
}

void MainController::loadDescriptions(int trj_num, string filename)
{
    cv::FileStorage file(filename, cv::FileStorage::READ);

    if (!file.isOpened())
    {
        throw MainController::NoFileExist(filename);
    }

    auto &descriptions = model->getTrajectory(trj_num).descriptions;
    descriptions.clear();

    int n = 0;
    file["count"] >> n;
    for (int i = 0; i < n; i++)
    {
        descriptions.push_back(cv::Mat());
        file[to_string(i)] >> descriptions[i];
    }
    file.release();
}

void MainController::saveDescriptions(int trj_num, string filename)
{
    const auto &descriptions = model->getTrajectory(trj_num).descriptions;

    cv::FileStorage file(filename, cv::FileStorage::WRITE);

    int n = descriptions.size();
    file << "count" << n;
    for (int i = 0; i < descriptions.size(); i++)
    {
        file << "img"+to_string(i) << descriptions[i];
    }
    file.release();
}

void MainController::updateTrajectoryKeyPointsCloud(int trj_num)
{
    auto &selected_frames = trajectories_selected_frames[trj_num];
    auto &key_points = model->getTrajectory(trj_num).key_points;
    std::vector<cv::KeyPoint> &cloud = trajectories_kp_cloud[trj_num];
    cloud.clear();

    if (!key_points.empty())
    {
        for (auto& frame_num: selected_frames)
        {
            cloud.insert(cloud.end(), key_points[frame_num].begin(), key_points[frame_num].end());
        }
    }
    qDebug() << "trj_num = " << trj_num << " size: " << cloud.size();
}

void MainController::initDetectors()
{
    detectors_names.push_back("SIFT");
    detectors.push_back(cv::xfeatures2d::SIFT::create());

    detectors_names.push_back("SURF");
    detectors.push_back(cv::xfeatures2d::SURF::create());

    detectors_names.push_back("KAZE");
    detectors.push_back(cv::KAZE::create());

    detectors_names.push_back("AKAZE");
    detectors.push_back(cv::AKAZE::create());

    detectors_names.push_back("BRISK");
    detectors.push_back(cv::BRISK::create());

    detectors_names.push_back("ORB");
    detectors.push_back(cv::ORB::create());
}

void MainController::initDescriptors()
{
    descriptors_names.push_back("SIFT");
    descriptors.push_back(cv::xfeatures2d::SIFT::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("SURF");
    descriptors.push_back(cv::xfeatures2d::SURF::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("KAZE");
    descriptors.push_back(cv::KAZE::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("AKAZE");
    descriptors.push_back(cv::AKAZE::create());
    norm_types.push_back(cv::NORM_L2SQR);

    descriptors_names.push_back("BRISK");
    descriptors.push_back(cv::BRISK::create());
    norm_types.push_back(cv::NORM_HAMMING);

    descriptors_names.push_back("ORB");
    descriptors.push_back(cv::ORB::create());
    norm_types.push_back(cv::NORM_HAMMING);

    descriptors_names.push_back("FREAK");
    descriptors.push_back(cv::xfeatures2d::FREAK::create());
    norm_types.push_back(cv::NORM_HAMMING);
}

void MainController::clear()
{
    for (int i = 0; i < trajectories_kp_cloud.size(); i++)
    {
        trajectories_kp_cloud[i].clear();
        trajectories_selected_frames[i].clear();
    }
}

Trajectory MainController::loadTrjFromCsv(string csv_filename)
{        
    Trajectory trj;
    try
    {
        auto parsed_csv = utils::csvtools::read_csv(csv_filename);

        size_t found = csv_filename.find_last_of("/\\");
        string folder = csv_filename.substr(0,found);

        for (auto &row : parsed_csv)
        {
            if (row[0] == "Path")
            {//column names
                continue;
            }
            row[0] = folder + "/" + row[0];
            Map frame = loadMapFromRow(row);
            trj.frames.push_back(frame);
        }
    }
    catch (runtime_error er)
    {
        showException(er.what());
    }


    return trj;
}

Map MainController::loadMapFromRow(std::vector<string> params)
{
    //need for atof, because '.' is not delimeter of float part
    char *saved_locale;
    saved_locale = setlocale(LC_NUMERIC, "C");


    double x_m = atof(params[1].c_str()); //QString::fromStdString(params[1]).toDouble();
    double y_m = atof(params[2].c_str()); //QString::fromStdString(params[2]).toDouble();

    double angle = atof(params[3].c_str()); //QString::fromStdString(params[3]).toDouble();
    double m_per_px = atof(params[4].c_str()); //QString::fromStdString(params[4]).toDouble();


    setlocale(LC_NUMERIC, saved_locale);

    return Map(params[0], x_m, y_m, angle, m_per_px);
}
