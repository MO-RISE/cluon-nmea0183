/// Copyright 2021 RISE Research Institute of Sweden. All rights reserved.
#ifndef SRC_NMEA0183_ASSEMBLER_HPP_
#define SRC_NMEA0183_ASSEMBLER_HPP_

#include <chrono>  // NOLINT
#include <functional>
#include <sstream>
#include <string>
#include <utility>

class NMEA0183SentenceAssembler {
  std::string remainder_;
  std::function<void(const std::string &,
                     const std::chrono::system_clock::time_point &)>
      delegate_{};

 public:
  NMEA0183SentenceAssembler(
      std::function<void(const std::string &,
                         const std::chrono::system_clock::time_point &)>
          delegate)
      : delegate_(delegate) {}

  void operator()(std::string &&data,
                  std::chrono::system_clock::time_point &&tp) {
    // Get total buffered data
    std::stringstream buffer{remainder_ + data};

    const std::chrono::system_clock::time_point time_stamp{std::move(tp)};

    // Handle any fully received lines
    std::string line;
    while (std::getline(buffer, line, '\n')) {
      if (buffer.eof()) {
        // If we run out of buffer, we dont have a full line and bail
        // accordingly
        break;
      }
      if (line.at(0) == '$' || line.at(0) == '!') {
        // All valid nmea messages should start with either $ or !
        // Trim for any trailing whitespace
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        // Pass on to delegate
        delegate_(line, time_stamp);
      }
    }
    // Make sure we save any remaining characters for next incoming
    remainder_ = line;
  }
};

#endif  // SRC_NMEA0183_ASSEMBLER_HPP_"
