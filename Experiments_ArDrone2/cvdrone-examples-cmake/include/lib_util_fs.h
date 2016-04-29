/*
 * fsutil.h
 *
 *  Created on: 20.09.2012
 *      Author: ar
 */

#ifndef _LIB_FSUTIL_H_
#define _LIB_FSUTIL_H_


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <cstring>

#include <vector>

#define MAX_CNT        8192 // max number files in directory for reading

#ifdef __linux__
    #define SEP_FILES    '/'
#elif defined _WIN32
    #define SEP_FILES    '\\'
#endif

#ifdef __linux__
    #include <cstdio>
    #include <sys/time.h>
    class SimpleTimer_UNIX {
     private:
        timeval     startTime;
        timeval     stopTime;
        double      duration;
     public:
        void start() {
            gettimeofday(&startTime, NULL);
        }
        double stop()  {
            gettimeofday(&stopTime, NULL);
            duration = (stopTime.tv_sec-startTime.tv_sec)*1000.0 + (stopTime.tv_usec-startTime.tv_usec)/1000.0;
            return duration;
        }
        void printTime() {
            printf("%5.6fms\n", duration);
        }
        void stopAndPrint() {
            stop();
            printTime();
        }
        void stopAndPrint(const std::string& msg) {
            stop();
            printf("%s: %5.6fms\n", msg.c_str(),  duration);
        }
    };
#elif defined _WIN32
#include <cv.h>
#include <highgui.h>
class SimpleTimer_WIN32 {
 private:
    double     startTime;
    double     stopTime;
    double      duration;
 public:
    void start() {
        startTime   = 1000.* ((double)cv::getTickCount()) / cv::getTickFrequency();
    }
    double stop()  {
        stopTime    = 1000.* ((double)cv::getTickCount()) / cv::getTickFrequency();
        duration    = stopTime - startTime;
        return duration;
    }
    void printTime() {
        printf("%5.6fms\n", duration);
    }
    void stopAndPrint() {
        stop();
        printTime();
    }
    void stopAndPrint(const string& msg) {
        stop();
        printf("%s: %5.6fms\n", msg.c_str(),  duration);
    }
};
#endif

#ifdef __linux__
    typedef SimpleTimer_UNIX  SimpleTimer;
#elif defined _WIN32
    typedef SimpleTimer_WIN32 SimpleTimer;
#endif

//////////////////////////////////////////////////////////////////////////
// string operations
void split_str(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ", bool trimToken=false);
void strTrim(std::string& s, const char* delim = " \t");
bool strStartWith(const std::string& base, const std::string& str);
bool strEndWith(const std::string& base, const std::string& str);
// filesystem-operations
std::string getFileNameFromPath(const std::string& path);
std::string getDirNameFromPath(const std::string& path);
std::string getFileExt(const std::string& fileName);
void parsePath(const std::string& path, std::string& dirName, std::string& fileName, std::string& ext);
bool isFileHasExt(const std::string& fileName, const std::string& ext);
bool fileGood(const std::string& f);
bool dirGood(const std::string& dir);

bool makePath( std::string path );
void listFilesInDir(const std::string& dirName, std::vector<std::string>& fileList, const std::string& ext = "", bool skipDir=true);
void listDirInDir(const std::string& dirName, std::vector<std::string>& dirList, const std::string& startS="", const std::string& endS="",  bool skipHidden=true, bool retFullPath=true);

// file-operations
bool readStringListFromFile(const std::string& fileName, std::vector<std::string>& stringList, bool isSkipStr=true);

//////////////////////////////////////////////////////////////////////////
#ifdef __linux__
    #include <dirent.h>
    #include <sys/stat.h>
    #include <errno.h>
    void listFilesInDirUNIX(const std::string& dirName, std::vector<std::string>& fileList, const std::string& ext = "", bool skipDir=true) {
        fileList.clear();
        DIR* dir;
        dir    = opendir(dirName.c_str());
        struct dirent *ent;
        if(dir!=NULL) {
            int cnt    = 0;
            bool isCheckExt    = false;
            if(ext.size()>0) {
                isCheckExt    = true;
            }
            while( ( ent=readdir(dir) )!=NULL ) {
                if(skipDir && (ent->d_type==DT_DIR) )
                    continue;
                std::string fn    = dirName + "/" + ent->d_name;
                if(isCheckExt) {
                    if(isFileHasExt(fn, ext)) {
                        fileList.push_back(fn);
                    }
                } else {
                    fileList.push_back(fn);
                }
                cnt++;
                if(cnt>MAX_CNT) {
                    std::cerr << ":: max number of director-list iterations exceeded..." << std::endl;
                    break;
                }
            }
            closedir(dir);
        } else {
            std::cerr << "Can't read directory [" << dirName << "]" << std::endl; // FIXME: for DEGUG version
        }
    }
    void listDirInDirUNIX(const std::string& dirName, std::vector<std::string>& dirList, const std::string& startS, const std::string& endS,  bool skipHidden, bool retFullPath) {
        dirList.clear();
        DIR* dir;
        dir    = opendir(dirName.c_str());
        struct dirent *ent;
        if(dir!=NULL) {
            int cnt    = 0;
            bool isCheckStart    = false;
            bool isCheckEnd        = false;
            if(startS.size()>0) {
                isCheckStart    = true;
            }
            if(endS.size()>0) {
                isCheckEnd    = true;
            }
            while( ( ent=readdir(dir) )!=NULL ) {
                if(ent->d_type==DT_DIR ) {
                    std::string tmpDirName    = ent->d_name;
                    if(skipHidden && tmpDirName[0]=='.') continue;
                    if(isCheckStart) {
                        if(!strStartWith(tmpDirName, startS)) continue;
                    }
                    if(isCheckEnd) {
                        if(!strEndWith(tmpDirName, endS)) continue;
                    }
                    if(retFullPath) {
                        dirList.push_back(dirName +"/" + tmpDirName);
                    } else {
                        dirList.push_back(tmpDirName);
                    }
                }
                cnt++;
                if(cnt>MAX_CNT) {
                    std::cerr << ":: max number of director-list iterations exceeded..." << std::endl;
                    break;
                }
            }
            closedir(dir);
        } else {
            std::cerr << "Can't read directory [" << dirName << "]" << std::endl; // FIXME: for DEGUG version
        }
    }
    bool dirGoodUNIX(const std::string& dir) {
        bool ret    = false;
        DIR* dirId;
        dirId    = opendir(dir.c_str());
        if(dirId!=NULL) {
            ret    = true;
        }
        closedir(dirId);
        return ret;
    }
    bool makePathUNIX( std::string path ) {
        bool bSuccess = false;
        int nRC = ::mkdir( path.c_str(), 0775 );
        if( nRC == -1 ) {
            switch( errno ) {
                case ENOENT:
                    //parent didn't exist, try to create it
                    if( makePath( path.substr(0, path.find_last_of('/')) ) )
                    //Now, try to create again.
                        bSuccess = 0 == ::mkdir( path.c_str(), 0775 );
                    else
                        bSuccess = false;
                    break;
                case EEXIST:
                    //Done!
                    bSuccess = true;
                    break;
                default:
                    bSuccess = false;
                    break;
            }
        }
        else
            bSuccess = true;
        return bSuccess;
    }
#elif defined _WIN32
    #include <windows.h>
    #include <conio.h>
    bool checkFile(const string& fname) {
        bool ret    = false;
        if(fname.size()>4) {
            ret    = true;
        }
        return ret;
    }
    void listFilesInDirWIN32(const string& directoryName, vector<string>& listFileNames, const std::string& ext = "", bool skipDir=true) {
        WIN32_FIND_DATAA    wfd;
        string dirmask    = directoryName+"/*.*";
        HANDLE hFind    = FindFirstFileA(dirmask.c_str(), &wfd);
        string buff;
        listFileNames.clear();
        bool isGood        = true;
        bool isCheckExt    = false;
        if(ext.size()>0) {
            isCheckExt    = true;
        }
        int cnt    = 0;
        if(hFind!=INVALID_HANDLE_VALUE) {
            do {
                if(wfd.cFileName[0] != '.') {
                    string fn    = directoryName + "\\" + wfd.cFileName;
                    if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                        if(skipDir)
                            continue;
                    }
                    if(isCheckExt) {
                        if(isFileHasExt(fn, ext)) {
                            listFileNames.push_back(fn);
                        }
                    } else {
                        listFileNames.push_back(fn);
                    }
                }
                cnt++;
                if(cnt>MAX_CNT) {
                    cerr << ":: max number of director-list iterations exceeded..." << std::endl;
                    break;
                }
            } while(FindNextFileA(hFind, &wfd)==TRUE);
        }
    }
    void listDirInDirWIN32(const std::string& dirName, std::vector<std::string>& dirList, const std::string& startS="", const std::string& endS="",  bool skipHidden=true, bool retFullPath=true) {
        dirList.clear();
        WIN32_FIND_DATAA    wfd;
        string dirmask    = dirName+"/*.*";
        HANDLE hFind    = FindFirstFileA(dirmask.c_str(), &wfd);
        string buff;
        int cnt    = 0;
        if(hFind!=INVALID_HANDLE_VALUE) {
            bool isCheckStart    = false;
            bool isCheckEnd        = false;
            if(startS.size()>0) {
                isCheckStart    = true;
            }
            if(endS.size()>0) {
                isCheckEnd    = true;
            }
            do {
                if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                    string tmpDirName    = wfd.cFileName;
                    if(skipHidden && tmpDirName[0]=='.') continue;
                    if(isCheckStart) {
                        if(!strStartWith(tmpDirName, startS)) continue;
                    }
                    if(isCheckEnd) {
                        if(!strEndWith(tmpDirName, endS)) continue;
                    }
                    if(retFullPath) {
                        dirList.push_back(dirName + SEP_FILES + tmpDirName);
                    } else {
                        dirList.push_back(tmpDirName);
                    }
                    cnt++;
                    if(cnt>MAX_CNT) {
                        cerr << ":: max number of director-list iterations exceeded..." << std::endl;
                        break;
                    }
                }
            } while(FindNextFileA(hFind, &wfd)==TRUE);
        }
    }
    bool dirGoodWIN32(const std::string& dir) {
        return !((GetFileAttributesA(dir.c_str())) == INVALID_FILE_ATTRIBUTES);
    }
    bool makePathWIN32(std::string path) {
        bool bSuccess = false;
        int nRC = CreateDirectory(path.c_str(), NULL);
        int err = GetLastError();
        if( nRC == 0 ) {
            switch( err ) {
                case ERROR_PATH_NOT_FOUND:
                    //parent didn't exist, try to create it
                    if( makePathWIN32( path.substr(0, path.find_last_of(SEP_FILES)) ) )
                        //Now, try to create again.
                        bSuccess = (0 != CreateDirectory( path.c_str(), NULL ));
                    else
                        bSuccess = false;
                    break;
                case ERROR_ALREADY_EXISTS:
                    //Done!
                    bSuccess = true;
                    break;
                default:
                    bSuccess = false;
                    break;
            }
        }
        else
            bSuccess = true;
        return bSuccess;
    }
#endif

//////////////////////////////////////////////////////////////////////////
// String
void split_str(const std::string& str,
                      std::vector<std::string>& tokens,
                      const std::string& delimiters,
                      bool trimToken/*=false*/) {
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
        if(trimToken) {
            std::string tmp = str.substr(lastPos, pos - lastPos);
            strTrim(tmp);
            tokens.push_back(tmp);
        } else {
            tokens.push_back(str.substr(lastPos, pos - lastPos));
        }
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void strTrim(std::string& s, const char* delim) {
    s.erase(0,s.find_first_not_of(delim));
    s.erase(s.find_last_not_of(delim) + 1, std::string::npos);
}

bool strStartWith(const std::string& base, const std::string& str) {
    return (base.find(str)==0);
}

bool strEndWith(const std::string& base, const std::string& str) {
    unsigned int idx    = base.rfind(str) + str.size();
    return (idx==base.size());
}

// File-system
void listFilesInDir(
        const std::string& dirName,
        std::vector<std::string>& fileList,
        const std::string& ext,
        bool skipDir) {
#ifdef __linux__
    listFilesInDirUNIX(dirName, fileList, ext, skipDir);
#elif defined _WIN32
    listFilesInDirWIN32(dirName, fileList, ext, skipDir);
#endif
}

void listDirInDir(
        const std::string& dirName,
        std::vector<std::string>& dirList,
        const std::string& startS,
        const std::string& endS,
        bool skipHidden,
        bool retFullPath) {
#ifdef __linux__
    listDirInDirUNIX(dirName, dirList, startS, endS,  skipHidden, retFullPath);
#elif defined _WIN32
    listDirInDirWIN32(dirName, dirList, startS, endS,  skipHidden, retFullPath);
#endif
}

std::string getFileNameFromPath(const std::string& path) {
    char sep    = SEP_FILES;
    size_t idx  = path.rfind(sep, path.length());
    if(idx!=std::string::npos) {
        return path.substr(idx+1, path.length()-1);
    }
    return (path);
}

std::string getDirNameFromPath(const std::string& path) {
    char sep    = SEP_FILES;
    size_t idx  = path.rfind(sep, path.length());
    if(idx!=std::string::npos) {
        return path.substr(0, idx);
    }
    return ("");
}

std::string getFileExt(const std::string& fileName) {
    // small hack for path like: "./dir/file_without_ext"
    char sep    = SEP_FILES;
    size_t idxd    = fileName.rfind(sep, fileName.length());
    size_t idx  = fileName.rfind('.', fileName.length());
    if(idx!=std::string::npos) {
        if(idxd!=std::string::npos) {
            if(idx<idxd) {
                return ("");
            }
        }
        return fileName.substr(idx+1, fileName.length()-1);
    }
    return ("");
}

void parsePath(const std::string& path, std::string& dirName, std::string& fileName, std::string& ext) {
    dirName     = getDirNameFromPath(path);
    std::string tmpFileName    = getFileNameFromPath(path);
    size_t idx  = tmpFileName.rfind('.', tmpFileName.length()-1);
    if(idx!=std::string::npos) {
        ext    = tmpFileName.substr(idx+1, tmpFileName.length()-1);
    } else {
        ext    = "";
    }
    if(idx>0) {
        fileName    = tmpFileName.substr(0,idx);
    } else {
        fileName    = "";
    }
}

bool isFileHasExt(const std::string& fileName, const std::string& ext) {
    if((ext.size() + 1)>fileName.size()) {
        return false;
    } else {
        std::string rext = getFileExt(fileName);
        if(rext==ext) {
            return true;
        }
    }
    return false;
}

bool fileGood(const std::string& f) {
    std::ifstream ifs(f.c_str());
    return ifs.good();
}

bool dirGood(const std::string& dir) {
#ifdef __linux__
    return dirGoodUNIX(dir);
#elif defined _WIN32
    return dirGoodWIN32(dir);
#endif
}

bool makePath( std::string path ) {
#ifdef __linux__
    return makePathUNIX(path);
#elif defined _WIN32
    return makePathWIN32(path);
#endif
}

// file-IO
bool readStringListFromFile(const std::string& fileName, std::vector<std::string>& stringList, bool isSkipStr) {
    bool ret    = false;
    stringList.clear();
    std::ifstream ifs;
    ifs.open(fileName.c_str());
    if(!ifs) {
        std::cerr << "Can't read a file [" << fileName << "]" << std::endl;
    } else {
        std::string str;
        while(std::getline(ifs, str)) {
            strTrim(str);
            if(isSkipStr) {
                if(str.size()>0 && str[0]!='#') {
                    stringList.push_back(str);
                }
            } else {
                if(str.size()>0) {
                    stringList.push_back(str);
                }
            }
        }
        ret = true;
    }
    return ret;
}

#endif /* FSUTIL_H_ */
