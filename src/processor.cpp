#include <vector>
#include <string>
#include <numeric>
#include <iostream>

#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {

  vector<string> utilizationStrings = LinuxParser::CpuUtilization();
  vector<int> cpuTimes;

  std::transform(utilizationStrings.begin(), utilizationStrings.end(),
                 std::back_inserter(cpuTimes),
                 [](const std::string& str) { return std::stoi(str); });

  const int idle = cpuTimes[3];
  const int total = std::accumulate(cpuTimes.begin(), cpuTimes.end(), 0);

  auto idle_delta = (float) (idle - previous_idle);
  auto total_delta = (float) (total - previous_total);

  this->previous_idle = idle;
  this->previous_total = total;

  return 1 - idle_delta / total_delta;
}
