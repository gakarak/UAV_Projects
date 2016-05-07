#ifndef GRADIENT_DENSITY_H
#define GRADIENT_DENSITY_H

#include <algorithm>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace utils
{

    namespace cv
    {

        double gradientDensity(::cv::Mat image)
        {
            ::cv::Point2f center(image.cols / 2, image.rows / 2);
            int radius = std::min(image.cols, image.rows) / 2;

            ::cv::Mat grad_x;
            ::cv::Mat grad_y;

            ::cv::Sobel(image, grad_x, CV_16S, 1, 0);
            ::cv::Sobel(image, grad_y, CV_16S, 0, 1);

            ::cv::Mat abs_grad_x;
            ::cv::Mat abs_grad_y;

            ::cv::convertScaleAbs(grad_x, abs_grad_x);
            ::cv::convertScaleAbs(grad_y, abs_grad_y);

            ::cv::Mat grad_value;
            ::cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad_value);

            double grad_density = 0;
            int pixel_count = 0;

            for (int i = 0; i < grad_value.rows; i++)
            {
                for (int j = 0; j < grad_value.cols; j++)
                {
                    if (sqrt(pow(i - center.y, 2) + pow(j - center.x, 2)) < radius)
                    {
                        grad_density += grad_value.at<uchar>(i, j);
                        pixel_count++;
                    }
                }
            }

            return grad_density / pixel_count;
        }

    }

}

#endif // GRADIENT_DENSITY_H
