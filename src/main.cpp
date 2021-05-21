/// Copyright 2021 RISE Research Institute of Sweden. All rights reserved.
#include <iostream>

#include "NMEA0183_assembler.hpp"
#include "cluon-complete.hpp"
#include "cxxopts.hpp"
#include "spdlog/sinks/daily_file_sink.h"

auto main(int argc, char **argv) -> int {
  cxxopts::Options options("NMEA0183 Sentence Assembler",
                           "Eavesdrops on a NMEA0183 stream and publishes each "
                           "raw sentence to a running OpenDaVINCI session.");

  auto add = options.add_options();
  add("c,cid", "OpenDaVINCI session id", cxxopts::value<int>());
  add("a,address", "IP address of stream", cxxopts::value<std::string>());
  add("p,port", "Port number to connect to", cxxopts::value<int>());
  add("v,verbose", "Print to cout");
  add("u,udp", "Use an UDP connection");

  auto arguments = options.parse(argc, argv);
  const bool VERBOSE = arguments["verbose"].as<bool>();
  const uint16_t cid = arguments["cid"].as<uint16_t>();
  const std::string address = arguments["address"].as<std::string>();
  const uint16_t port = arguments["port"].as<uint16_t>();
  const bool use_udp = arguments["udp"].as<bool>();

  // Setup a cluon instance
  cluon::OD4Session od4{cid,
                        [](auto) {}};  // Empty callback for incoming messages.

  // A sink that rotates at midnight
  auto logger =
      spdlog::daily_logger_mt("daily_logger", "NMEA0183/daily.txt", 0, 0);
  logger->set_pattern("%v");

  // Wrap everything in a sentence handler lambda
  auto sentence_handler =
      [&VERBOSE, &logger, &od4](
          const std::string &sentence,
          const std::chrono::system_clock::time_point &timestamp) {
        // TODO(freol35241): Publish message

        // Log to disk
        std::stringstream message;
        message << timestamp.time_since_epoch().count() << " " << sentence;
        logger->info(message.str());

        if (VERBOSE) {
          std::cout << message.str() << std::endl;
        }
      };

  // And finally use this in a SentenceAssembler
  NMEA0183SentenceAssembler assembler(sentence_handler);

  if (use_udp) {
    cluon::UDPReceiver fromDevice{
        address, port,
        [&assembler](std::string &&d, std::string && /*from*/,
                     std::chrono::system_clock::time_point &&tp) noexcept {
          assembler(std::move(d), std::move(tp));
        }};
  } else {
    // Interface to a TCP stream with incoming NMEA0183 messages
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
