#ifndef SAVEABLEFLANNMATCHER_H
#define SAVEABLEFLANNMATCHER_H

#include <vector>
#include <string>

#include <opencv2/xfeatures2d.hpp>

using namespace std;

class SaveableFlannMatcher : public cv::FlannBasedMatcher
{
 public:
  SaveableFlannMatcher()
  {
  }

  virtual ~SaveableFlannMatcher()
  {
  }

  void printParams()
  {
      printf("SaveableMatcher::printParams: \n\t"
          "addedDescCount=%d\n\t"
          "flan distance_t=%d\n\t"
          "flan algorithm_t=%d\n",
          addedDescCount,
          flannIndex->getDistance(),
          flannIndex->getAlgorithm());

      /*vector<std::string> names;
      vector<int> types;
      vector<std::string> strValues;
      vector<double> numValues;

      indexParams->getAll(names, types, strValues, numValues);

      for (size_t i = 0; i < names.size(); i++)
          printf("\tindex param: %s:\t type=%d val=%s %.2f\n",
                  names[i].c_str(), types[i],
                  strValues[i].c_str(), numValues[i]);

      names.clear();
      types.clear();
      strValues.clear();
      numValues.clear();
      searchParams->getAll(names, types, strValues, numValues);

      for (size_t i = 0; i < names.size(); i++)
          printf("\tsearch param: %s:\t type=%d val=%s %.2f\n",
                  names[i].c_str(), types[i],
                  strValues[i].c_str(), numValues[i]);*/
  }

  void readIndex(const char* filename)
  {
      indexParams->setAlgorithm(cvflann::FLANN_INDEX_SAVED);
      indexParams->setString("filename", filename);

      // construct flannIndex now, so printParams works
      train();

      printParams();
  }

  void writeIndex(const char* filename)
  {
      printParams();
      flannIndex->save(filename);
  }
};

#endif // SAVEABLEFLANNMATCHER_H
