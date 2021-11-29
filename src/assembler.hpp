///  Copyright 2021 RISE Research Institute of Sweden - Maritime Operations
///
///  Licensed under the Apache License, Version 2.0 (the "License");
///  you may not use this file except in compliance with the License.
///  You may obtain a copy of the License at
///
///      http://www.apache.org/licenses/LICENSE-2.0
///
///  Unless required by applicable law or agreed to in writing, software
///  distributed under the License is distributed on an "AS IS" BASIS,
///  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///  See the License for the specific language governing permissions and
///  limitations under the License.
#ifndef SRC_ASSEMBLER_HPP_
#define SRC_ASSEMBLER_HPP_

#include <chrono>  // NOLINT
#include <functional>
#include <sstream>
#include <string>
#include <utility>

class Assembler {
  std::string remainder_;
  std::function<void(std::string &&, std::chrono::system_clock::time_point &&)>
      delegate_{};

 public:
  Assembler(std::function<void(std::string &&,
                               std::chrono::system_clock::time_point &&)>
                delegate)
      : delegate_(delegate) {}

  void operator()(const std::string &&data,
                  std::chrono::system_clock::time_point &&tp) {
    // Get total buffered data
    std::stringstream buffer{remainder_ + std::move(data)};

    std::chrono::system_clock::time_point time_stamp{std::move(tp)};

    // Handle any fully received lines
    std::string line;
    while (std::getline(buffer, line, '\n')) {
      if (buffer.eof()) {
        // If we run out of buffer, we dont have a full line and bail
        // accordingly
        break;
      }
      // Trim for any trailing whitespace
      line.erase(line.find_last_not_of(" \n\r\t") + 1);
      // Pass on to delegate
      delegate_(std::move(line), std::move(time_stamp));
    }
    // Make sure we save any remaining characters for next incoming
    remainder_ = line;
  }
};

#endif  // SRC_ASSEMBLER_HPP_"
