
#include <chrono>
#include <functional>
#include <sstream>
#include <string>

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
      : delegate_(delegate){};

  void operator()(std::string &&data,
                  std::chrono::system_clock::time_point &&tp) {
    // Get total buffered data
    std::stringstream buffer{remainder_ + data};

    const std::chrono::system_clock::time_point time_stamp{std::move(tp)};

    // Handle any fully received lines
    std::string line;
    while (std::getline(buffer, line, '\n')) {
      if (buffer.eof()) {
        // If we run out of buffer, lets keep that last partial and bail
        remainder_ = line;
        break;
      }
      if (line.at(0) == '$' or line.at(0) == '!') {
        // All valid nmea messages should start with either $ or !
        // Trim for any trailing whitespace
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        // Pass on to delegate
        delegate_(line, time_stamp);
      }
    }
  }
};
