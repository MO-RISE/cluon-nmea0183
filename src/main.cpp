/// Copyright 2021 RISE Research Institute of Sweden. All rights reserved.
#include <iostream>

#include "CLI/CLI.hpp"
#include "NMEA0183_assembler.hpp"
#include "cluon-complete.hpp"
#include "spdlog/sinks/daily_file_sink.h"

auto main(int argc, char **argv) -> int {
  CLI::App app{"NMEA0183 Eavesdropper"};

  uint16_t cid;
  app.add_option("-c,--cid", cid, "OpenDaVINCI session id")->required();
  std::string address;
  app.add_option("-a,--address", address, "IP address of stream")->required();
  uint16_t port;
  app.add_option("-p,--port", port, "Port number to connect to")->required();
  bool use_udp = false;
  app.add_option("--udp", use_udp, "Use an UDP connection");
  bool verbose = false;
  app.add_option("--verbose", verbose, "Print to cout");

  CLI11_PARSE(app, argc, argv);

  // Setup a cluon instance
  cluon::OD4Session od4{cid,
                        [](auto) {}};  // Empty callback for incoming messages.

  // A sink that rotates at midnight
  auto logger =
      spdlog::daily_logger_mt("daily_logger", "NMEA0183/sentences.txt", 0, 0);
  logger->set_pattern("%v");

  // Wrap everything in a sentence handler lambda
  auto sentence_handler =
      [&verbose, &logger, &od4](
          const std::string &sentence,
          const std::chrono::system_clock::time_point &timestamp) {
        // TODO(freol35241): Publish message

        // Log to disk
        std::stringstream message;
        message << timestamp.time_since_epoch().count() << " " << sentence;
        logger->info(message.str());

        if (verbose) {
          std::cout << message.str() << std::endl;
        }
      };

  // And finally use this in a SentenceAssembler
  NMEA0183SentenceAssembler assembler(sentence_handler);

  // Setup a connection to aither a udp or tcp stream with incoming NMEA0183
  // messages
  if (use_udp) {
    cluon::UDPReceiver fromDevice{
        address, port,
        [&assembler](std::string &&d, std::string && /*from*/,
                     std::chrono::system_clock::time_point &&tp) noexcept {
          assembler(std::move(d), std::move(tp));
        }};
  } else {
    cluon::TCPConnection stream{
        address, port, std::ref(assembler), [&argv]() {
          std::cerr << "[" << argv[0] << "] Connection lost." << std::endl;
          exit(1);
        }};
  }

  // Just sleep as this microservice is data driven.
  using namespace std::literals::chrono_literals;  // NOLINT
  while (od4.isRunning()) {
    std::this_thread::sleep_for(1s);
  }

  return 0;
}
