#include "ardrone.h"
#include <opencv2/stitching/stitcher.hpp>

// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // AR.Drone class
    ARDrone ardrone;

    // Initialize
    if (!ardrone.open()) {
        printf("Failed to initialize.\n");
        return -1;
    }

    // Snapshots
    std::vector<cv::Mat> snapshots;

    // Key frame
    cv::Mat last = cv::Mat(ardrone.getImage(), true);

    // ORB detector/descriptor
    cv::OrbFeatureDetector detector;
    cv::OrbDescriptorExtractor extractor;

    // Main loop
    while (1) {
        // Key input
        int key = cv::waitKey(1);
        if (key == 0x1b) break;

        // Update
        if (!ardrone.update()) break;

        // Get an image
        cv::Mat image = cv::Mat(ardrone.getImage());

        // Detect key points
        cv::Mat descriptorsA, descriptorsB;
        std::vector<cv::KeyPoint> keypointsA, keypointsB;
        detector.detect(last, keypointsA);
        detector.detect(image, keypointsB);
        extractor.compute(last, keypointsA, descriptorsA);
        extractor.compute(image, keypointsB, descriptorsB);

        // Match key points
        std::vector<cv::DMatch> matches;
        cv::BFMatcher matcher(cv::NORM_HAMMING, true);
        matcher.match(descriptorsA, descriptorsB, matches);

        // Count matches
        int count = 0;
        for (int i = 0; i < (int)matches.size(); i++) {
            if (matches[i].queryIdx == matches[i].trainIdx) count++; // Yet, strange way
        }

        // Take a snapshot when scene was changed
        if (count == 0) {
            image.copyTo(last);
            cv::Ptr<cv::Mat> tmp(new cv::Mat());
            image.copyTo(*tmp);
            snapshots.push_back(*tmp);
        }

        // Display the image
        cv::Mat matchImage;
        cv::drawMatches(last, keypointsA, image, keypointsB, matches, matchImage, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        cv::imshow("camera", matchImage);
    }

    // Stiching
    cv::Mat result;
    cv::Stitcher stitcher = cv::Stitcher::createDefault();
    printf("Stitching images...\n");
    if (stitcher.stitch(snapshots, result) == cv::Stitcher::OK) {
        cv::imshow("result", result);
        cv::imwrite("result.jpg", result);
        cv::waitKey(0);
    }

    // See you
    ardrone.close();

    return 0;
}