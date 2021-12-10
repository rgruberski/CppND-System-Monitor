#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <numeric>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// DONE: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;

  std::filesystem::path dir {kProcDirectory};

  if (std::filesystem::exists(dir) and std::filesystem::is_directory(dir))
  {
    for (auto& e : std::filesystem::directory_iterator(dir))
    {
      string filename(e.path().stem());

      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        pids.push_back(stoi(filename));
      }
    }
  }

  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string label;
  float value;
  float total = 0;
  float free = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> label >> value) {
        if (label == "MemTotal:") {
          total = value;
        }
        else if (label == "MemFree:") {
          free = value;
        }
      }
    }
  }

  return free / total;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime = 0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return uptime;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {

  vector<string> utilizationStrings = CpuUtilization();
  vector<int> cpuTimes;

  std::transform(utilizationStrings.begin(), utilizationStrings.end(),
                 std::back_inserter(cpuTimes),
                 [](const std::string& str) { return std::stoi(str); });

  return std::accumulate(cpuTimes.begin(), cpuTimes.end(), 0);
}

// DONE: Read and return the number of active jiffies for a PID
// DONE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  return std::stol(ReadStats(pid)[13]) + std::stol(ReadStats(pid)[14])
      + std::stol(ReadStats(pid)[15]) + std::stol(ReadStats(pid)[16]);
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> utilizationStrings = CpuUtilization();
  return std::stoi(utilizationStrings[3]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string key;
  vector<std::string> cpuValues;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      linestream >> key;

      if (key == "cpu") {
        do
        {
          string cpuValue;
          linestream >> cpuValue;
          if (!cpuValue.empty()) {
            cpuValues.push_back(cpuValue);
          }
        } while (linestream);
      }
    }
  }
  return cpuValues;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the command associated with a process
// DONE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  string path = kProcDirectory + to_string(pid) + kCmdlineFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

// DONE: Read and return the memory used by a process
// DONE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {

  string line;
  string label;
  string value;

  string path = kProcDirectory + to_string(pid) + kStatusFilename;

  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> label >> value) {
        if (label == "VmSize:") {
          return to_string(std::stoi(value) / 1024);
        }
      }
    }
  }

  return {};
}

// DONE: Read and return the user ID associated with a process
// DONE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string uid;

  string path = kProcDirectory + to_string(pid) + kStatusFilename;

  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> uid) {
        if (key == "Uid:") {
          return uid;
        }
      }
    }
  }

  return {};
}

// DONE: Read and return the user associated with a process
// DONE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string passwdGroup;
  string passwdUsername;
  string passwdUid;

  std::ifstream filestreamPasswd(kPasswordPath);
  if (filestreamPasswd.is_open()) {
    while (std::getline(filestreamPasswd, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> passwdUsername >> passwdGroup >> passwdUid) {
        if (passwdUid == Uid(pid)) {
          return passwdUsername;
        }
      }
    }
  }

  return {};
}

// DONE: Read and return the uptime of a process
// DONE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  return UpTime() - std::stol(ReadStats(pid)[21]) /
                        sysconf(_SC_CLK_TCK);
}

// DONE: Read all values from a stat file of a process
vector<string> LinuxParser::ReadStats(int pid) {

  string line;
  string value;
  vector<string> stats;
  string path = kProcDirectory + to_string(pid) + kStatFilename;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      line.erase(line.find('(') + 1,
                 line.find(')') - line.find('(') - 1);
      std::istringstream linestream(line);
      while (linestream >> value)
      {
        stats.push_back(value);
      }
    }
  }

  return stats;
}