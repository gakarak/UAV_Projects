#include "ardrone.h"
#include "lib_util_fs.h"

#include <ctime>
#include <iostream>
#include <sstream>

using namespace std;

//////////////////////////////////
string getCurrentVideoFileName();
void on_track_animation_type(int pos, void*);
void drawContrastText(cv::Mat& img, const string& txt, const cv::Point& ptx);

//////////////////////////////////
static const int animation_types[][2] = {
    {ARDRONE_ANIM_PHI_M30_DEG,              1000},
    {ARDRONE_ANIM_PHI_30_DEG,               1000},
    {ARDRONE_ANIM_THETA_M30_DEG,            1000},
    {ARDRONE_ANIM_THETA_30_DEG,             1000},
    {ARDRONE_ANIM_THETA_20DEG_YAW_200DEG,   1000},
    {ARDRONE_ANIM_THETA_20DEG_YAW_M200DEG,  1000},
    {ARDRONE_ANIM_TURNAROUND,               5000},
    {ARDRONE_ANIM_TURNAROUND_GODOWN,        5000},
    {ARDRONE_ANIM_YAW_SHAKE,                2000},
    {ARDRONE_ANIM_YAW_DANCE,                5000},
    {ARDRONE_ANIM_PHI_DANCE,                5000},
    {ARDRONE_ANIM_THETA_DANCE,              5000},
    {ARDRONE_ANIM_VZ_DANCE,                 5000},
    {ARDRONE_ANIM_WAVE,                     5000},
    {ARDRONE_ANIM_PHI_THETA_MIXED,          5000},
    {ARDRONE_ANIM_DOUBLE_PHI_THETA_MIXED,   5000},
    {ARDRONE_ANIM_FLIP_AHEAD,               15},
    {ARDRONE_ANIM_FLIP_BEHIND,              15},
    {ARDRONE_ANIM_FLIP_LEFT,                15},
    {ARDRONE_ANIM_FLIP_RIGHT,               15}
};
static const char *animation_types_names[] = {
    "ARDRONE_ANIM_PHI_M30_DEG",
    "ARDRONE_ANIM_PHI_30_DEG",
    "ARDRONE_ANIM_THETA_M30_DEG",
    "ARDRONE_ANIM_THETA_30_DEG",
    "ARDRONE_ANIM_THETA_20DEG_YAW_200DEG",
    "ARDRONE_ANIM_THETA_20DEG_YAW_M200DEG",
    "ARDRONE_ANIM_TURNAROUND",
    "ARDRONE_ANIM_TURNAROUND_GODOWN",
    "ARDRONE_ANIM_YAW_SHAKE",
    "ARDRONE_ANIM_YAW_DANCE",
    "ARDRONE_ANIM_PHI_DANCE",
    "ARDRONE_ANIM_THETA_DANCE",
    "ARDRONE_ANIM_VZ_DANCE",
    "ARDRONE_ANIM_WAVE",
    "ARDRONE_ANIM_PHI_THETA_MIXED",
    "ARDRONE_ANIM_DOUBLE_PHI_THETA_MIXED",
    "ARDRONE_ANIM_FLIP_AHEAD",
    "ARDRONE_ANIM_FLIP_BEHIND",
    "ARDRONE_ANIM_FLIP_LEFT",
    "ARDRONE_ANIM_FLIP_RIGHT"
};
static const int animation_types_num = sizeof(animation_types)/sizeof(animation_types[0]);

//////////////////////////////////
struct SNavDataState {
    SNavDataState(int frameIdx=-1, double timeInSec=-1., int flagAnimationStart=-1) {
        init(frameIdx, timeInSec, flagAnimationStart);
    }
    void getDroneState(ARDrone& drone,
                       int frameIdx=-1,
                       double timeInSec=-1.,
                       int flagAnimationStart=-1) {
        this->p_frameIdx    = frameIdx;
        this->p_TimeInSec   = timeInSec;
        this->p_FlagAnimationStart  = flagAnimationStart;
        //
        this->p_batteryPercent    = drone.getBatteryPercentage();
        this->p_Roll        = drone.getRoll();
        this->p_Pitch       = drone.getPitch();
        this->p_Yaw         = drone.getYaw();
        this->p_Vmag        = drone.getVelocity(&p_Vx, &p_Vy, &p_Vz);
        this->p_Altitude    = drone.getAltitude();
//        drone.getPosition(&p_PosLatitude, &p_PosLongitude, &p_PosElevation);
        drone.getACC(&p_PosLatitude, &p_PosLongitude, &p_PosElevation);
        this->p_time        = drone.getTime();
    }
    std::string toCSVString() const {
        std::stringstream ss;
        ss << p_frameIdx << ", " << p_TimeInSec << ", " << p_FlagAnimationStart << ", "
           << p_batteryPercent << ", "
           << p_Roll << ", "
           << p_Pitch << ", "
           << p_Yaw << ", "
           << p_Vx << ", "
           << p_Vy << ", "
           << p_Vz << ", "
           << p_Vmag << ", "
           << p_Altitude << ", "
           << p_PosLatitude << ", "
           << p_PosLongitude << ", "
           << p_PosElevation << ", "
           << p_time;
        return ss.str();
    }
    std::string toString() const {
        return toCSVString();
    }
    void init(int frameIdx=-1, double timeInSec=-1., int flagAnimationStart=-1) {
        p_frameIdx      =  frameIdx;
        p_TimeInSec     =  timeInSec;
        p_FlagAnimationStart    = flagAnimationStart;
        p_Roll          = -1.0;
        p_Pitch         = -1.0;
        p_Yaw           = -1.0;
        p_Vx            =  0.0;
        p_Vy            =  0.0;
        p_Vz            =  0.0;
        p_Vmag          = -1.0;
        p_PosLatitude   =  0.0;
        p_PosLongitude  =  0.0;
        p_PosElevation  =  0.0;
    }
    bool loadFromString(const std::string& str) {
        std::vector<std::string> spl;
        split_str(str, spl, ",");
        if(spl.size()<15) {
            init();
            return false;
        } else {
            p_frameIdx          = atoi(spl[0].c_str());
            p_TimeInSec         = atoi(spl[1].c_str());
            p_FlagAnimationStart= atoi(spl[2].c_str());
            p_batteryPercent    = atoi(spl[3].c_str());
            p_Roll              = atoi(spl[4].c_str());
            p_Pitch             = atoi(spl[5].c_str());
            p_Yaw               = atoi(spl[6].c_str());
            p_Vx                = atoi(spl[7].c_str());
            p_Vy                = atoi(spl[8].c_str());
            p_Vz                = atoi(spl[9].c_str());
            p_Vmag              = atoi(spl[10].c_str());
            p_Altitude          = atoi(spl[11].c_str());
            p_PosLatitude       = atoi(spl[12].c_str());
            p_PosLongitude      = atoi(spl[13].c_str());
            p_PosElevation      = atoi(spl[14].c_str());
            return true;
        }
    }
    static std::string getHeader() {
        return "#BEGIN frameIdx[0], TimeInSec[1], FlagAnimationStart[2], batteryPercent[3], Roll[4], Pitch[5], Yaw[6], Vx[7], Vy[8], Vz[9], Vmag[10], Altitude[11], PosLatitude(ax)[12], PosLongitude(ay)[13], PosElevation(az)[14], DroneTime[15]";
    }
    static std::string getFooter() {
        return "#END ";
    }
    //
    int     p_frameIdx;             // 0
    double  p_TimeInSec;            // 1
    int     p_FlagAnimationStart;   // 2
    double  p_batteryPercent;       // 3
    double  p_Roll;                 // 4
    double  p_Pitch;                // 5
    double  p_Yaw;                  // 6
    double  p_Vx;                   // 7
    double  p_Vy;                   // 8
    double  p_Vz;                   // 9
    double  p_Vmag;                 // 10
    double  p_Altitude;             // 11
    double  p_PosLatitude;          // 12
    double  p_PosLongitude;         // 13
    double  p_PosElevation;         // 14
    double  p_ax;                   //
    double  p_ay;                   //
    double  p_az;                   //
    int     p_time;                 //
    //
    friend std::ostream& operator<< (std::ostream& out, const SNavDataState& v);
};
std::ostream& operator<<(std::ostream& out, const SNavDataState& v) {
    out << v.toString();
    return out;
}

class SNavDataWriter {
public:
    SNavDataWriter() : currentFilePath(""), writeCounter(0) {}
    bool isOpened() const {
        return ofs.is_open();
    }
    void release() {
        if(ofs.is_open()) {
            ofs << SNavDataState::getFooter() << std::endl;
            ofs.close();
        }
        currentFilePath = "";
        writeCounter    = 0;
    }
    bool open(const std::string& filePath) {
        release();
        ofs.open(filePath.c_str());
        if(ofs) {
            currentFilePath = filePath;
            ofs << SNavDataState::getHeader() << std::endl;
            return true;
        } else {
            std::cerr << "Can't open file [" << filePath << "]" << std::endl;
            currentFilePath = "";
        }
        return false;
    }
    bool writeNavDataState(const SNavDataState& s) {
        if(isOpened()) {
            ofs << s.toCSVString() << std::endl;
            writeCounter++;
        } else {
            std::cerr << "ERROR: can't write state: stream is not opened!" << std::endl;
            return false;
        }
    }
    bool writeDroneState(ARDrone& drone,
                         int    frameIdx=-1,
                         double timeInSec=-1.,
                         int    flagAnimationStart=-1) {
        SNavDataState state;
        state.getDroneState(drone, frameIdx, timeInSec, flagAnimationStart);
        return writeNavDataState(state);
    }
    std::string toString() const {
        std::stringstream ss;
        ss << "SNavDataWriter{ isOpened=" << isOpened()
           << ", file=[" << currentFilePath
           << "], writed states=" << writeCounter << "}";
        return ss.str();
    }
    friend std::ostream& operator<< (std::ostream& out, const SNavDataWriter& v);
private:
    std::ofstream   ofs;
    std::string     currentFilePath;
    int             writeCounter;
};
std::ostream& operator<<(std::ostream& out, const SNavDataWriter& v) {
    out << v.toString();
    return out;
}

//////////////////////////////////
#define DEF_DELAY   33
#define DEF_FPS     (1000./DEF_DELAY)
#define DEF_WIN     "drone-video"

bool isFirstRun         = true;
bool isNewVideoFile     = true;
bool isStateRecording   = false;
cv::Size    frameSize(0,0);
int currentAnimationIdx = 0;
SimpleTimer timer;

//////////////////////////////////
int main(int argc, char **argv)
{
    ARDrone ardrone;
    if (!ardrone.open()) {
        printf("Failed to initialize.\n");
        return -1;
    }

    cv::namedWindow(DEF_WIN);
    cv::createTrackbar("Drone ANIMATION: ", DEF_WIN, &currentAnimationIdx, animation_types_num-1, on_track_animation_type);

    cv::VideoWriter writerVideo;
    SNavDataWriter  writerData;
    int cntBadFrames        =  0;
    int cntGoodFrames       =  0;
    int flagStartAnimation  = -1;
    while (true) {
        int key = cvWaitKey(DEF_DELAY);
        if (key == 27) break;
        if (!ardrone.update()) break;
        cv::Mat frame   = cv::Mat(ardrone.getImage());
        if(!frame.data) {
            cntBadFrames++;
            cerr << "bad frame #" << cntBadFrames << endl;
            continue;
        }
        cntGoodFrames++;
        if(isFirstRun) {
            frameSize.width     = frame.cols;
            frameSize.height    = frame.rows;
            isFirstRun          = true;
        }
        if(isStateRecording) {
            if(isNewVideoFile) {
                if(writerVideo.isOpened()) {
                    writerVideo.release();
                }
                string fileNameVideo = getCurrentVideoFileName();
                string fileNameData  = fileNameVideo + ".csv";
                writerVideo.open(getCurrentVideoFileName(), CV_FOURCC('D','I','V','X'), DEF_FPS, frameSize, true);
                writerData.open(fileNameData);
                isNewVideoFile  = false;
                timer.start();
            }
            writerVideo << frame;
            writerData.writeDroneState(ardrone, cntGoodFrames, timer.stop(), flagStartAnimation);
        }
        cv::Mat buff    = frame.clone();
        cv::line(buff, cv::Point(frameSize.width/2, 0), cv::Point(frameSize.width/2,frameSize.height), cv::Scalar(0,0,255), 1, CV_AA);
        cv::line(buff, cv::Point(0,frameSize.height/2), cv::Point(frameSize.width,frameSize.height/2), cv::Scalar(0,0,255), 1, CV_AA);
        stringstream ss;
        ss << std::fixed;
        ss.precision(2);
        ss << "battery: " << ardrone.getBatteryPercentage() << "%";
        drawContrastText(buff, ss.str(), cv::Point(10,20+20*0));
        ss.str("");
        ss << "animation: " << animation_types_names[currentAnimationIdx];
        drawContrastText(buff, ss.str(), cv::Point(10,20+20*1));
        {
            double tmpLatitude, tmpLongitude, tmpElevation;
//            ardrone.getPosition(&tmpLatitude, &tmpLongitude, &tmpElevation);
            ardrone.getACC(&tmpLatitude, &tmpLongitude, &tmpElevation);
            ss.str("");
            ss << "position (Lat,Lon,Elv): {" << tmpLatitude << ", " << tmpLongitude << ", " << tmpElevation << "}, time: " << ardrone.getTime();
            drawContrastText(buff, ss.str(), cv::Point(10,20+20*2));
        }
        ss.str("");
        ss << "position (Roll,Pitch,Yaw): {" << ardrone.getRoll() << ", " << ardrone.getPitch() << ", " << ardrone.getYaw() << "}";
        drawContrastText(buff, ss.str(), cv::Point(10,20+20*3));
        ss.str("");
        ss << "Altitude: " << ardrone.getAltitude() << " [m]";
        drawContrastText(buff, ss.str(), cv::Point(10,20+20*4));
        {
            double tmpVx, tmpVy, tmpVz, tmpVm;
            tmpVm = ardrone.getVelocity(&tmpVx, &tmpVy, &tmpVz);
            ss.str("");
            ss << "Velocity: {" << tmpVx << ", " << tmpVy << ", " << tmpVz << "}, " << tmpVm <<  " [m/s]";
            drawContrastText(buff, ss.str(), cv::Point(10,20+20*5));
        }
        //
        if(isStateRecording) {
            cv::circle(buff, cv::Point(frameSize.width-100,20), 5, cv::Scalar(0,0,255), -1, CV_AA);
            cv::putText(buff, "Rec", cv::Point(frameSize.width-100+10,30), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0,0,255), 2, CV_AA);
        }
        cv::imshow(DEF_WIN, buff);
        double vx = 0.0;
        double vy = 0.0;
        double vz = 0.0;
        double vr = 0.0;
        flagStartAnimation  = -1;
        if(key=='r') {
            cout << "record" << endl;
            if(isStateRecording) {
                isStateRecording    = false;
            } else {
                isStateRecording    = true;
                if(!writerVideo.isOpened()) {
                    isNewVideoFile  = true;
                }
                timer.start();
            }
        }
        if(key=='n') {
            isNewVideoFile  = true;
            cout << "new-video" << endl;
        }
        if(key==' ') {
            if (ardrone.onGround())
                ardrone.takeoff();
            else
                ardrone.landing();
        }
        if(key=='p') {
            flagStartAnimation  = animation_types[currentAnimationIdx][0];
            ardrone.setAnimation(animation_types[currentAnimationIdx][0], animation_types[currentAnimationIdx][1]);
        }
        if (key=='w' /*KEY_UP*/)      vx =  1.0;
        if (key=='s' /*KEY_DOWN*/)    vx = -1.0;
        if (key=='a' /*KEY_LEFT*/)    vr =  1.0;
        if (key=='d' /*KEY_RIGHT*/)   vr = -1.0;
        if (key == 'z')         vz =  1.0;
        if (key == 'x')         vz = -1.0;
        ardrone.move3D(vx, vy, vz, vr);
        static int mode = 0;
        if (key == 'c') {
            ardrone.setCamera(++mode%4);
            isFirstRun      = true;
            isNewVideoFile  = true;
        }
    }
    if(writerVideo.isOpened()) {
        writerVideo.release();
    }
    writerData.release();
    ardrone.close();
    return 0;
}

//////////////////////////////////
string getCurrentVideoFileName() {
    stringstream ss;
    time_t rawtime;
    struct tm * st;
    time (&rawtime);
    st = localtime(&rawtime);
    ss << "video.drone_"  << st->tm_year << "."
                          << st->tm_mon  << "."
                          << st->tm_mday << "."
                          << st->tm_hour << "."
                          << st->tm_min  << "."
                          << st->tm_sec  << ".avi";
    return ss.str();
}

void on_track_animation_type(int pos, void*) {
    cout << "animation #" << pos << " : " << animation_types_names[pos] << endl;
}

void drawContrastText(cv::Mat& img, const string& txt, const cv::Point& ptx) {
    cv::putText(img, txt, ptx, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0,  0,  0  ), 3, CV_AA);
    cv::putText(img, txt, ptx, CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255,255,255), 1, CV_AA);
}
