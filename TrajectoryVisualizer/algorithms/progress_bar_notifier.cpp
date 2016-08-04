#include "progress_bar_notifier.h"

using namespace algorithmspkg;

void algorithmspkg::ProgressBarNotifier::setCallback(NotifyFunc callback)
{
  this->callback = callback;
}

void ProgressBarNotifier::notifyProgressBar(int value, int maximum, int minimum)
{
  callback(value, maximum, minimum);
}
