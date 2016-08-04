#ifndef PROGRESSBARNOTIFIER_H
#define PROGRESSBARNOTIFIER_H

#include <tr1/functional>

namespace algorithmspkg
{

class ProgressBarNotifier
{
 public:
  using NotifyFunc = std::tr1::function<void(int, int, int)>;

  ProgressBarNotifier() = default;
  /**
   * @brief setCallback
   * @param callback - function, which accept value, maximum, minimum and notify
   * a progress bar
   */
  void setCallback(NotifyFunc callback);

 protected:
  void notifyProgressBar(int value, int maximum = 100, int minimum = 0);

 private:
  NotifyFunc callback;
};

}

#endif // PROGRESSBARNOTIFIER_H
