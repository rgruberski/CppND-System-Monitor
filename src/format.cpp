#include <string>
#include <iostream>
#include <iomanip>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {

  long outDays = seconds / (24 * 3600);

  seconds = seconds % (24 * 3600);
  long outHours = seconds / 3600;

  seconds %= 3600;
  long outMinutes = seconds / 60 ;

  seconds %= 60;
  long outSeconds = seconds;

  std::ostringstream oss;
  oss << outDays << " days, "
      << std::setw(2) << std::setfill('0') << outHours << ":"
      << std::setw(2) << std::setfill('0') << outMinutes << ":"
      << std::setw(2) << std::setfill('0') << outSeconds << "\n";

  return oss.str();
}