#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace modelpkg
{
    struct Map
    {
        Map() {}

        Map(std::string filename, double x_m, double y_m, double angle, double m_per_px)
            : image( cv::imread(filename, cv::IMREAD_GRAYSCALE) ), pos_m(x_m, y_m),
              angle(angle), m_per_px(m_per_px)
        {
          image_center = cv::Point2f(image.cols/2., image.rows/2.);
        }

        cv::Mat image;
        cv::Point2f image_center;
        cv::Point2f pos_m; //in meters
        double angle;
        double m_per_px;
    };
}

#endif // MAP_H
