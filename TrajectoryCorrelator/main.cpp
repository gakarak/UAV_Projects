#include <iostream>

#include <QString>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "algorithms/trajectory_loader.h"
#include "utils/image_transforms.h"

using namespace algorithmspkg;
using namespace modelpkg;

void printUsing();
void visualizeCorrelation(cv::Mat backward, cv::Mat templ, cv::Mat result);


int main(int argc, char *argv[]){

  if (argc != 5 && argc != 6){
    printUsing();
    return 1;
  }

  TrajectoryLoader loader;
  Trajectory forward = loader.loadTrajectory(argv[1]);
  Trajectory backward = loader.loadTrajectory(argv[2]);
  double scale_step = atof(argv[3]);
  double rotate_step = atof(argv[4]);
  int match_method = CV_TM_CCOEFF_NORMED;
  std::string output_name = "result.png";

  if (argc == 6)
  {
    output_name = argv[5];
  }

  for (int b_i = backward.getFramesCount() - 1; b_i >= 0; b_i--){
    for (int f_i = 0; f_i < forward.getFramesCount(); f_i++){
      const auto &b_frame = backward.getFrame(b_i);
      const auto &f_frame = forward.getFrame(f_i);

      cv::imshow("forward", f_frame.image);

      double glob_max = 0;
      cv::Mat maxtempl;
      cv::Mat maxresult;

      double scale = 1;
      while (scale > 0.4){
        double angle = 0;
        while (angle < 360){
          auto templ = cv::scaleRotateCropImage(f_frame.image, scale, angle);

          cv::Mat result;
          cv::matchTemplate(b_frame.image, templ, result, match_method);

          double local_min, local_max;
          cv::minMaxLoc(result, &local_min, &local_max);
          if (local_max > glob_max)
          {
            maxtempl = templ;
            maxresult = result;
            glob_max = local_max;
          }

          angle += rotate_step;
        }
        scale *= scale_step;
      }

      visualizeCorrelation(b_frame.image, maxtempl, maxresult);
    }
  }

  /*cv::imshow("Original", forward.getFrame(0).image);

  for (double scale = 1; scale >= 0.5; scale -= 0.1){
    for (int angle = 0; angle < 360; angle += 20){
      cv::imshow("ScaleRotateCrop",
                 cv::scaleRotateCropImage(forward.getFrame(0).image,
                                          scale, angle)
                 );
      cv::waitKey(0);
    }
  }*/


  return 0;
}

void visualizeCorrelation(cv::Mat backward, cv::Mat templ, cv::Mat result){
  //out score
  double min, max;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(result, &min, &max, &minLoc, &maxLoc);
  QString status = QString("Max: %1").arg(QString::number(max));
  auto font = cv::FONT_HERSHEY_SIMPLEX;
  cv::putText(result, status.toStdString().c_str(),
              cv::Point(20, 40), font, 1, cv::Scalar::all(4));

  cv::Mat back_copy;
  backward.copyTo(back_copy);

  cv::rectangle( back_copy, maxLoc,
                 cv::Point( maxLoc.x + templ.cols , maxLoc.y + templ.rows ),
                 cv::Scalar::all(0), 2, 8, 0 );

  cv::imshow("Backward", back_copy);
  cv::imshow("Template", templ);
  cv::imshow("Result", result);
  cv::waitKey(0);

}

void printUsing(){
  std::cout << "Using: \n" <<
               "TrajectoryCorrelator forward_way_csv backward_way_csv "
               "scale_step "
               "rotate_step "
               "[output_name=result.png]"
            << std::endl;
  std::cout << "\n\tforward_way_csv - flight on the low height" << std::endl;
  std::cout << "\n\tbackward_way_csv - flight on the high height" << std::endl;
  std::cout << "\n\tscale_step - scaling for forward_way <1" << std::endl;
  std::cout << "\n\trotate_step - in degrees" << std::endl;
  std::cout << "\n\toutput_name - name for output correlation map" << std::endl;
  std::cout << std::endl;
}
