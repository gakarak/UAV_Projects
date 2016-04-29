//#include "stdafx.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <iostream>

using namespace cv;
using namespace std;

static void fftShift(InputOutputArray _out)
{
    Mat out = _out.getMat();

    if(out.rows == 1 && out.cols == 1)
    {
        // trivially shifted.
        return;
    }

    vector<Mat> planes;
    split(out, planes);

    int xMid = out.cols >> 1;
    int yMid = out.rows >> 1;

    bool is_1d = xMid == 0 || yMid == 0;

    if(is_1d)
    {
        xMid = xMid + yMid;

        for(size_t i = 0; i < planes.size(); i++)
        {
            Mat tmp;
            Mat half0(planes[i], Rect(0, 0, xMid, 1));
            Mat half1(planes[i], Rect(xMid, 0, xMid, 1));

            half0.copyTo(tmp);
            half1.copyTo(half0);
            tmp.copyTo(half1);
        }
    }
    else
    {
        for(size_t i = 0; i < planes.size(); i++)
        {
            // perform quadrant swaps...
            Mat tmp;
            Mat q0(planes[i], Rect(0,    0,    xMid, yMid));
            Mat q1(planes[i], Rect(xMid, 0,    xMid, yMid));
            Mat q2(planes[i], Rect(0,    yMid, xMid, yMid));
            Mat q3(planes[i], Rect(xMid, yMid, xMid, yMid));

            q0.copyTo(tmp);
            q3.copyTo(q0);
            tmp.copyTo(q3);

            q1.copyTo(tmp);
            q2.copyTo(q1);
            tmp.copyTo(q2);
        }
    }

    merge(planes, out);
} 

void findMagnitude(InputArray _src, OutputArray _dst)
{
	Mat mag = _dst.getMat();
	Mat padded;
	Mat f = _src.getMat();
	int m = getOptimalDFTSize(f.rows);
	int n = getOptimalDFTSize(f.cols);
	copyMakeBorder(f, padded, 0, m - f.rows, 0, n - f.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
	Mat complexI;
	merge(planes, 2, complexI);

	dft(complexI, complexI);

	split(complexI, planes);
	magnitude(planes[0], planes[1], planes[0]);
	Mat tmp = planes[0];
	mag = tmp;
}

int main(int, char* [])
{
    int key = 0;

	float accum_x = 0.0f;
	float accum_y = 0.0f;
	float accum_angle = 0.0f;
	float accum_scale = 1.0f;

//    VideoCapture video("/home/ar/dev-git.git/dev.opencv/VideoNav9_CMake/data/video.avi");
    VideoCapture video(0);
	Mat frame1, frame2, frame1_crop, frame2_crop, FFT1, FFT2, FFT1m, FFT2m, hann, f1, f2, frame1_crop_gray, frame2_crop_gray, FFT1c, FFT2c, frame2_crop_gray_rot;
    Mat map(30000,30000, CV_8UC3);
//	imshow("map", map);

	Mat track(1000,1000, CV_8UC3);

	float k = 5.0f;

/*	video.read(frame1);
	for (int i=0;i<200;i++)
	{
		video.read(frame2);
	}*/
    bool isStarted = false;
	int n = 100;
    do
    {
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
		if (!video.read(frame2)) break;
        if(frame1.empty())
        {
            frame1 = frame2.clone();
        }
//        if(!isStarted) {
//            imshow("cameraView", frame2);
//            key = waitKey(10);
//            if(key==32) {
//                std::cout << "SPACE" << std::endl;
//                isStarted = true;
//            }
//            continue;
//        }

/*		frame1 = cv::imread("image1.png", CV_LOAD_IMAGE_COLOR);
		frame2 = cv::imread("image2.png", CV_LOAD_IMAGE_COLOR);*/

//		imshow("frame1", frame1);
//		imshow("frame2", frame2);

		int size = min(frame1.cols,frame1.rows);
///		int size = 256;
		Rect roi;
		roi.x = frame1.cols/2 - size/2;
		roi.y = frame1.rows/2 - size/2;
		roi.width = size;
		roi.height = size;
		frame1_crop = frame1(roi);
		frame2_crop = frame2(roi);

		cvtColor(frame1_crop, frame1_crop_gray, CV_RGB2GRAY);
		cvtColor(frame2_crop, frame2_crop_gray, CV_RGB2GRAY);
		imshow("frame1cropGray", frame1_crop_gray);
//		imshow("frame2cropGray", frame2_crop_gray);

		frame1_crop_gray.convertTo(f1, CV_32F); 
		frame2_crop_gray.convertTo(f2, CV_32F);

// -----------------------------------------------------------------
		Mat padded1;
		int m1 = getOptimalDFTSize(f1.rows);
		int n1 = getOptimalDFTSize(f1.cols);
		copyMakeBorder(f1, padded1, 0, m1 - f1.rows, 0, n1 - f1.cols, BORDER_CONSTANT, Scalar::all(0));

		Mat planes1[] = {Mat_<float>(padded1), Mat::zeros(padded1.size(), CV_32F)};
		Mat complexI1;
		merge(planes1, 2, complexI1);

		dft(complexI1, complexI1);

		split(complexI1, planes1);
		magnitude(planes1[0], planes1[1], planes1[0]);
		FFT1 = planes1[0];

		fftShift(FFT1);

//		Mat FFT1g;
//		FFT1.convertTo(FFT1g,CV_8UC1, 1.0f/255.0f); 
//	    imshow("spectrum magnitude 1", FFT1g);

/*		FFT1 += Scalar::all(1);
		log(FFT1, FFT1);
		normalize(FFT1, FFT1, 0, 1, CV_MINMAX);
//	    imshow("spectrum magnitude 1", FFT1);*/

		/*Rect roi1;
		float s = 100.0f;
		roi1.x = 40;
		roi1.y = 0;
		roi1.width = 240;
		roi1.height = FFT1.rows;
		FFT1c = FFT1(roi1);
	    imshow("spectrum magnitude 1", FFT1c);*/

// -----------------------------------------------------------------
		Mat padded2;
		int m2 = getOptimalDFTSize(f2.rows);
		int n2 = getOptimalDFTSize(f2.cols);
		copyMakeBorder(f2, padded2, 0, m2 - f2.rows, 0, n2 - f2.cols, BORDER_CONSTANT, Scalar::all(0));

		Mat planes2[] = {Mat_<float>(padded2), Mat::zeros(padded2.size(), CV_32F)};
		Mat complexI2;
		merge(planes2, 2, complexI2);

		dft(complexI2, complexI2);

		split(complexI2, planes2);
		magnitude(planes2[0], planes2[1], planes2[0]);
		FFT2 = planes2[0];

		fftShift(FFT2);

/*		FFT2 += Scalar::all(1);
		log(FFT2, FFT2);
		normalize(FFT2, FFT2, 0, 1, CV_MINMAX);
//	    imshow("spectrum magnitude 2", FFT2);*/

/*		Rect roi2;
		s = 100;
		roi2.x = FFT2.cols/2 - s/2;
		roi2.y = FFT2.rows/2 - s/2;
		roi2.width = s;
		roi2.height = s;
		FFT2c = FFT2(roi2);
	    imshow("spectrum magnitude 2", FFT2c);*/

// -----------------------------------------------------------------

		float magnitude = 40.0f;
		Mat spectrum1_lp = cv::Mat::zeros(FFT1.size(), CV_32F);
		IplImage ispectrum1 = FFT1, ispectrum1_lp = spectrum1_lp;
		cvLogPolar(&ispectrum1, &ispectrum1_lp, cvPoint2D32f(FFT1.cols/2, FFT1.rows/2), magnitude);
	//    cvLogPolar(&ispectrum1, &ispectrum1_lp, cvPoint2D32f(0, 0), 40);

		Mat spectrum2_lp = cv::Mat::zeros(FFT2.size(), CV_32F);
		IplImage ispectrum2 = FFT2, ispectrum2_lp = spectrum2_lp;
		cvLogPolar(&ispectrum2, &ispectrum2_lp, cvPoint2D32f(FFT2.cols/2, FFT2.rows/2), magnitude);

		Rect roi_lp;
		roi_lp.x = 120;
		roi_lp.y = 0;
		roi_lp.width = 80;
		roi_lp.height = spectrum1_lp.rows;
		FFT1c = spectrum1_lp(roi_lp);
		FFT2c = spectrum2_lp(roi_lp);
///		imshow("spectrum magnitude 1", FFT1c);
///		imshow("spectrum magnitude 2", FFT2c);


	//	cv::imshow("frame1_lp", spectrum1_lp);
	//	cv::imshow("frame2_lp", spectrum2_lp);

		createHanningWindow(hann, spectrum1_lp.size(), CV_32F);
		cv::Point2d pt = cv::phaseCorrelate(spectrum1_lp, spectrum2_lp,hann);
		float Scale = exp(pt.x/magnitude);
		float Rotation = pt.y*360/(FFT1.cols);
		cout << "Scale = " << Scale << " Rotation = " << Rotation << std::endl;

//		createHanningWindow(hann, FFT1c.size(), CV_32F);
//		cv::Point2d pt = cv::phaseCorrelate(FFT1c, FFT2c);
/*		cv::Point2d pt = cv::phaseCorrelate(FFT1c, FFT2c,hann);
		float Scale = exp(pt.x/magnitude);
		float Rotation = pt.y*360/(FFT1.cols);
		cout << "Scale = " << Scale << " Rotation = " << Rotation << std::endl;*/

// -----------------------------------------------------------------

		// find offset
		Mat rot_matrix = getRotationMatrix2D(cvPoint2D32f(frame2_crop_gray.cols/2, frame2_crop_gray.rows/2), Rotation, Scale);
		warpAffine(frame2_crop_gray, frame2_crop_gray_rot, rot_matrix, frame2_crop_gray.size());

		size = 128;
		roi.x = frame1_crop_gray.cols/2 - size/2;
		roi.y = frame1_crop_gray.rows/2 - size/2;
		roi.width = size;
		roi.height = size;
		Mat frame1stich = frame1_crop_gray(roi);

		size = 128;
		roi.x = frame2_crop_gray_rot.cols/2 - size/2;
		roi.y = frame2_crop_gray_rot.rows/2 - size/2;
		roi.width = size;
		roi.height = size;
		Mat frame2stich = frame2_crop_gray_rot(roi);
///		imshow("rotation scale", frame2stich);

		Mat f1offset,f2offset;
		frame1_crop_gray.convertTo(f1offset, CV_32F); 
		frame2_crop_gray_rot.convertTo(f2offset, CV_32F);

		Mat hann_offset;
        createHanningWindow(hann_offset, f1offset.size(), CV_32F);
        Point2d shift = phaseCorrelate(f1offset, f2offset, hann_offset);

//		cout << "x = " << shift.x << " y = " << shift.y << std::endl;


		// find track


/*		accum_x+=shift.x;
		accum_y+=shift.y;*/
		accum_angle+=Rotation;
		accum_scale*=Scale;

		float xr = -(shift.x*cos(-accum_angle*3.14f/180.0f) - shift.y*sin(-accum_angle*3.14f/180.0f));
		float yr = -(shift.x*sin(-accum_angle*3.14f/180.0f) + shift.y*cos(-accum_angle*3.14f/180.0f));
		accum_x+=xr;
		accum_y+=yr;

		cout << "x = " << accum_x << " y = " << accum_y << std::endl;

		Mat frame2_global;
		Mat rot_matrix_global = getRotationMatrix2D(cvPoint2D32f(frame2_crop_gray.cols/2, frame2_crop_gray.rows/2), accum_angle, accum_scale);
		warpAffine(frame2, frame2_global, rot_matrix_global, frame2.size());
		imshow("result", frame2_global);

		Rect map_roi;
/*		map_roi.x = map.cols/2 + accum_x;
		map_roi.y = map.rows/2 + accum_y;
		map_roi.width = frame2_global.cols;
		map_roi.height = frame2_global.rows;*/
		map_roi.x = map.cols/2 + accum_x;
		map_roi.y = map.rows/2 + accum_y;
		map_roi.width = 400;
		map_roi.height = 400;
		Mat r = map(map_roi);

		Rect f_roi;
		f_roi.x = frame2_global.cols/2 - 200;
		f_roi.y = frame2_global.rows/2 - 200;
		f_roi.width = 400;
		f_roi.height = 400;
		Mat fr = frame2_global(f_roi);
//		frame2_global.copyTo(r);
		fr.copyTo(r);
		
		track.at<Vec3b>(Point(500.0f + accum_x/k,500.0f + accum_y/k))[0] = 255;
		track.at<Vec3b>(Point(500.0f + accum_x/k,500.0f + accum_y/k))[1] = 255;
		track.at<Vec3b>(Point(500.0f + accum_x/k,500.0f + accum_y/k))[2] = 255;
		imshow("track", track);

// -----------------------------------------------------------------

        key = waitKey(10);

//        if(key==32) {
//            std::cout << "SPACE" << std::endl;
//            isStarted = true;
//        }

        frame1 = frame2.clone();
    } while((char)key != 27); // Esc to exit...

	track.at<Vec3b>(Point(500.0f + accum_x/k,500.0f + accum_y/k))[0] = 255;
	track.at<Vec3b>(Point(500.0f + accum_x/k,500.0f + accum_y/k))[1] = 0;
	track.at<Vec3b>(Point(500.0f + accum_x/k,500.0f + accum_y/k))[2] = 0;

	track.at<Vec3b>(Point(500.0f,500.0f))[0] = 0;
	track.at<Vec3b>(Point(500.0f,500.0f))[1] = 0;
	track.at<Vec3b>(Point(500.0f,500.0f))[2] = 255;
	imshow("track", track);

    key = waitKey(500);

	imwrite("map.png",map);

    return 0;
}
