#include <unistd.h>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// DONE: Return this process's ID
int Process::Pid() { return pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  return ((float)LinuxParser::ActiveJiffies(pid)
          / (float)sysconf(_SC_CLK_TCK))
         / (float)LinuxParser::UpTime(pid);
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// DONE: Return this process's memory utilization
string Process::Ram() { ram = LinuxParser::Ram(pid); return ram; }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// DONE: Overload the "less than" comparison operator for Process objects
// DONE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {

  if (ram.empty()) {
    return false;
  }
  else if (a.ram.empty()) {
    return true;
  }

  return std::stoi(ram) < std::stoi(a.ram);
}

// DONE: Overload the "greater than" comparison operator for Process objects
// DONE: [[maybe_unused]] once you define the function
bool Process::operator>(Process const& a) const {

  if (ram.empty()) {
    return false;
  }
  else if (a.ram.empty()) {
    return true;
  }

  return std::stoi(ram) > std::stoi(a.ram);
}