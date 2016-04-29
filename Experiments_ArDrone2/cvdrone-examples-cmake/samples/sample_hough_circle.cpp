#include "ardrone.h"

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

    // Get a image
    IplImage* image = ardrone.getImage();

    // Images
    IplImage *gray   = cvCreateImage(cvGetSize(image), image->depth, 1);
    IplImage *smooth = cvCreateImage(cvGetSize(image), image->depth, 1);
    IplImage *canny  = cvCreateImage(cvGetSize(image), image->depth, 1);

    // Canny thresholds
    int th1 = 50, th2 = 100;
    cvNamedWindow("canny");
    cvCreateTrackbar("th1", "canny", &th1, 255);
    cvCreateTrackbar("th2", "canny", &th2, 255);

    // Main loop
    while (1) {
        // Key input
        int key = cvWaitKey(1);
        if (key == 0x1b) break;

        // Update
        if (!ardrone.update()) break;

        // Get an image
        image = ardrone.getImage();

        // Convert to gray scale
        cvCvtColor(image, gray, CV_BGR2GRAY);

        // De-noising
        cvSmooth(gray, smooth, CV_GAUSSIAN, 23, 23);

        // Detect edges
        cvCanny(smooth, canny, th1, th2, 3);

        // Detect circles
        CvMemStorage *storage = cvCreateMemStorage(0);
        CvSeq *circles = cvHoughCircles(smooth, storage, CV_HOUGH_GRADIENT, 1.0, 10.0, MAX(th1,th2), 20);

        // Draw circles
        for (int i = 0; i < circles->total; i++) {
            float *p = (float*) cvGetSeqElem(circles, i);
            cvCircle(image, cvPoint(cvRound(p[0]), cvRound(p[1])), cvRound(p[2]), CV_RGB(0,255,0), 3, 8, 0);
        }

        // Release memory
        cvReleaseMemStorage(&storage);

        // Change camera
        static int mode = 0;
        if (key == 'c') ardrone.setCamera(++mode%4);

        // Display the image
        cvShowImage("camera", image);
        cvShowImage("canny", canny);
    }

    // Release memories
    cvReleaseImage(&gray);
    cvReleaseImage(&smooth);
    cvReleaseImage(&canny);

    // See you
    ardrone.close();

    return 0;
}