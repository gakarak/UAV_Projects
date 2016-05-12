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

void MainController::calculateKeyPoints()
{
    cv::Ptr<cv::Feature2D> detector = cv::BRISK::create();

    model->first_trj.key_points.clear();
    for (const Map& map: model->first_trj.maps)
    {
        model->first_trj.key_points.push_back(vector<cv::KeyPoint>());
        detector->detect(map.image, model->first_trj.key_points.back());
    }

    for (auto &frame_kp: model->first_trj.key_points)
    {
        std::sort(frame_kp.begin(), frame_kp.end(), []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
                                         bool { return left.response > right.response; });
    }

    model->second_trj.key_points.clear();
    for (const Map& map: model->second_trj.maps)
    {
        model->second_trj.key_points.push_back(vector<cv::KeyPoint>());
        detector->detect(map.image, model->second_trj.key_points.back());
    }

    for (auto &frame_kp: model->second_trj.key_points)
    {
        std::sort(frame_kp.begin(), frame_kp.end(), []( const cv::KeyPoint &left, const cv::KeyPoint &right ) ->
                                         bool { return left.response > right.response; });
    }

    this->showFirstKeyPoints();
    this->showSecondKeyPoints();
}

void MainController::loadIni(string ini_filename)
{
    ConfigSingleton &cfg = ConfigSingleton::getInstance();
    try
    {
        cfg.loadIni(ini_filename);
        loadTrajectories(cfg.getPathToFirstTrajectoryCsv(),
                         cfg.getPathToSecondTrajectoryCsv());

        if (cfg.getPathToFirstKeyPointsBin() != "")
        {
            loadKeyPoints(cfg.getPathToFirstKeyPointsBin(), 0);
        }
        if (cfg.getPathToSecondKeyPointsBin() != "")
        {
            loadKeyPoints(cfg.getPathToSecondKeyPointsBin(), 1);
        }


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
    model->first_trj = loadTrjFromCsv(trj1_filename);
    model->second_trj = loadTrjFromCsv(trj2_filename);

    this->calculateMapsQuality();
    this->showFirstTrajectory();
    this->showSecondTrajectory();
}

void MainController::loadMainMap(string filename, double meters_per_pixel)
{
    model->main_map = modelpkg::Map(filename, 0, 0, 0, meters_per_pixel);

    double center_x_m = model->main_map.image.cols / 2.0 * meters_per_pixel;
    double center_y_m = model->main_map.image.rows / 2.0 * meters_per_pixel;
    model->main_map.pos_m = cv::Point2f(center_x_m, center_y_m);

    this->showMainMap();
}

void MainController::showFirstTrajectory()
{
    vector<QPixmap> qpixs;
    vector<QPointF> center_coords_px;
    vector<double>  angles;
    vector<double>  meter_per_pixels;
    vector<double> &qualities = model->first_trj.map_quality;

    for (const auto &map: model->first_trj.maps)
    {
        QPointF center_px;
        center_px.setX( map.pos_m.x / map.m_per_px );
        center_px.setY( map.pos_m.y / map.m_per_px );

        qpixs.push_back( utils::ASM::cvMatToQPixmap(map.image) );
        center_coords_px.push_back( center_px );
        angles.push_back( map.angle );
        meter_per_pixels.push_back( map.m_per_px );
    }

    view->setFirstTrajectory(qpixs, center_coords_px, angles, meter_per_pixels, qualities);
}

void MainController::showSecondTrajectory()
{
    vector<QPixmap> qpixs;
    vector<QPointF> center_coords_px;
    vector<double>  angles;
    vector<double>  meter_per_pixels;
    vector<double> &qualities = model->second_trj.map_quality;

    for (const auto &map: model->second_trj.maps)
    {
        QPointF center_px;
        center_px.setX( map.pos_m.x / map.m_per_px );
        center_px.setY( map.pos_m.y / map.m_per_px );

        qpixs.push_back( utils::ASM::cvMatToQPixmap(map.image) );
        center_coords_px.push_back( center_px );
        angles.push_back( map.angle );
        meter_per_pixels.push_back( map.m_per_px );
    }

    view->setSecondTrajectory(qpixs, center_coords_px, angles, meter_per_pixels, qualities);
}

void MainController::showMainMap()
{
    QPixmap qpix = utils::ASM::cvMatToQPixmap(model->main_map.image);

    view->setMainMap(qpix, model->main_map.m_per_px);
}

void MainController::showFirstKeyPoints()
{
    vector<vector<cv::KeyPoint>> &key_points = model->first_trj.key_points;
    vector<int> maps_num;
    vector<QPointF> center_coords_px;
    vector<double> angles;
    vector<double> radius;
    vector<QColor> colors;

    for (int i = 0; i < key_points.size(); i+=1)
    {
        for (int j = 0; j < std::min((size_t)100, key_points[i].size()); j++)
        {
            Map &map = model->first_trj.maps[i];
            cv::KeyPoint &kp = key_points[i][j];

            double mul = kp.response / 100;
            if (mul > 1) mul = 1;

            maps_num.push_back(i);
            center_coords_px.push_back(QPointF(kp.pt.x, kp.pt.y) - QPointF(map.image.cols/2, map.image.rows/2) + QPointF(map.pos_m.x / map.m_per_px, map.pos_m.y / map.m_per_px));
            angles.push_back(kp.angle);
            radius.push_back(kp.size / 2.);
            colors.push_back( QColor(255*mul, 255*(1-mul), 0) );//QColor(255*mul, 165*mul, 0));
        }
    }

    view->setFirstKeyPoints(maps_num, center_coords_px, angles, radius, colors);
}

void MainController::showSecondKeyPoints()
{
    vector<vector<cv::KeyPoint>> &key_points = model->second_trj.key_points;
    vector<int> maps_num;
    vector<QPointF> center_coords_px;
    vector<double> angles;
    vector<double> radius;
    vector<QColor> colors;

    for (int i = 0; i < key_points.size(); i+=1)
    {
        for (int j = 0; j < std::min((size_t)100, key_points[i].size()); j++)
        {
            Map &map = model->second_trj.maps[i];
            cv::KeyPoint &kp = key_points[i][j];

            double mul = kp.response / 160;
            if (mul > 1) mul = 1;

            maps_num.push_back(i);
            center_coords_px.push_back(QPointF(kp.pt.x, kp.pt.y) - QPointF(map.image.cols/2, map.image.rows/2) + QPointF(map.pos_m.x / map.m_per_px, map.pos_m.y / map.m_per_px));
            angles.push_back(kp.angle);
            radius.push_back(kp.size / 2.);
            colors.push_back( QColor(255*mul, 255*(1-mul), 0) );//QColor(255*mul, 165*mul, 0));
        }
    }

    view->setSecondKeyPoints(maps_num, center_coords_px, angles, radius, colors);
}

void MainController::saveKeyPoints(string filename, int trj_num)
{
    vector<vector<cv::KeyPoint>> &key_points = trj_num == 0? model->first_trj.key_points: model->second_trj.key_points;

    ofstream out(filename, ios::binary);
    size_t n = key_points.size();
    out.write(reinterpret_cast<char*>(&n), sizeof(n));
    for (int i = 0; i < key_points.size(); i++)
    {
        size_t m = key_points[i].size();
        out.write(reinterpret_cast<char*>(&m), sizeof(m));
        for (int j = 0; j < key_points[i].size(); j++)
        {
            out.write(reinterpret_cast<char*>(&key_points[i][j]), sizeof(key_points[i][j]));
        }
    }
}

void MainController::loadKeyPoints(string filename, int trj_num)
{
    ifstream in(filename, ios::binary);
    if (!in)
    {
        //bad show
        //because if it fails, then we can try to load second trajectory
        this->showException("Cannot open file: " + filename);
        return;
    }

    vector<vector<cv::KeyPoint>> &key_points = trj_num == 0? model->first_trj.key_points: model->second_trj.key_points;
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

    if (trj_num == 0)
    {
        this->showFirstKeyPoints();
    }
    else
    {
        this->showSecondKeyPoints();
    }
}

void MainController::showException(string what)
{
    clog << "Exception! " << what << endl;
    view->showException(QString::fromStdString(what));
}

/*
 * PRIVATE SECTION
 */

void MainController::calculateMapsQuality()
{
    double threshold = ConfigSingleton::getInstance().getQualityThreshold();

    //first trajectory
    for (const auto& map: model->first_trj.maps)
    {
        double scale = map.m_per_px / ConfigSingleton::getInstance().getGradientMetersPerPixel();
        cv::Size new_size(map.image.size().width * scale, map.image.size().height * scale);

        cv::Mat resized;
        cv::resize(map.image, resized, new_size);

        double quality = std::min( 1., utils::cv::gradientDensity(resized) / threshold );

        model->first_trj.map_quality.push_back(quality);
    }

    //second trajectory
    for (const auto& map: model->second_trj.maps)
    {
        double scale = map.m_per_px / ConfigSingleton::getInstance().getGradientMetersPerPixel();
        cv::Size new_size(map.image.size().width * scale, map.image.size().height * scale);

        cv::Mat resized;
        cv::resize(map.image, resized, new_size);

        double quality = std::min( 1., utils::cv::gradientDensity(resized) / threshold );

        model->second_trj.map_quality.push_back(quality);
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
            Map map = loadMapFromRow(row);
            trj.maps.push_back(map);
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
