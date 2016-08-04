#ifndef PROGRESSBARNOTIFIER_H
#define PROGRESSBARNOTIFIER_H

#include <tr1/functional>
#include <string>

namespace algorithmspkg
{

class ProgressBarNotifier
{
 public:
  using ProgressNotifier = std::tr1::function<void(int, int, int)>;
  using TitleSetter = std::tr1::function<void(std::string, bool)>;

  ProgressBarNotifier() = default;

  /**
   * @brief setTitleSetter
   * @param title_setter - function, which accept task_name and isNeedToReset
   * and set this params to progress bar
   */
  void setTitleSetter(TitleSetter title_setter);

  /**
   * @brief setProgressNotifier
   * @param progress_notifier - function, which accept value, maximum, minimum
   * and notifing the progress bar
   */
  void setProgressNotifier(ProgressNotifier progress_notifier);

 protected:
  void setProgressBarTitle(std::string title, bool isNeedToReset = true);
  void notifyProgressBar(int value, int maximum = 100, int minimum = 0);

 private:
  TitleSetter title_setter;
  ProgressNotifier progress_notifier;
};

}

#endif // PROGRESSBARNOTIFIER_H
