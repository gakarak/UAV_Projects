#include "progress_bar_notifier.h"

#include <iostream>

using namespace algorithmspkg;

void ProgressBarNotifier::setProgressNotifier(ProgressNotifier progress_notifier)
{
  this->progress_notifier = progress_notifier;
}

void ProgressBarNotifier::setTitleSetter(TitleSetter title_setter)
{
  this->title_setter = title_setter;
}




void ProgressBarNotifier::setProgressBarTitle(std::string title, bool isNeedToReset)
{
  if (title_setter != nullptr)
  {
    title_setter(title, isNeedToReset);
  }
  else
  {
    //do nothing
    //so client doesn't care about progress bar existence
  }
}

void ProgressBarNotifier::notifyProgressBar(int value, int maximum, int minimum)
{
  if (progress_notifier != nullptr)
  {
    progress_notifier(value, maximum, minimum);
  }
  else
  {
    //std::clog << "Warning: callback is nullptr!" << endl;
    //do nothing
    //so client doesn't care about progress bar existence
  }
}
