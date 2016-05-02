//
// trajectory_info.h
//
#ifndef TRAJECTORY_INFO_H
#define TRAJECTORY_INFO_H

#include <string>
#include <QRegExp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


struct TrajectoryInfo
{
	cv::Mat image;
	unsigned num;
    double x_m;
    double y_m;
    double angle;
    double m_per_px;

	size_t class_id;

    TrajectoryInfo() { }

    TrajectoryInfo(std::string filename, double x_meters, double y_meters, double angle, double m_per_px, size_t class_id)
        : class_id(class_id), x_m(x_meters), y_m(y_meters), angle(angle), m_per_px(m_per_px)
    {
        image = cv::imread(filename, cv::IMREAD_GRAYSCALE);

        QRegExp rx("(\\d+)");

        int pos = 0;
        QString res;
        while ((pos = rx.indexIn(filename.c_str(), pos)) != -1) {
            res = rx.cap(1);
            pos += rx.matchedLength();
        }

        num = res.toLong();
	}
};

#endif /* trajectory_info.h */
