/// Copyright 2021 RISE Research Institute of Sweden. All rights reserved.
#include <iostream>

#include "CLI/CLI.hpp"
#include "NMEA0183_assembler.hpp"
#include "cluon/OD4Session.hpp"
#include "cluon/UDPReceiver.hpp"
#include "spdlog/sinks/daily_file_sink.h"

auto run_as_gatherer(uint16_t cid, uint16_t id, std::string address,
                     uint16_t port, bool verbose) {
  // Setup a cluon instance
  cluon::OD4Session od4{cid,
                        [](auto) {}};  // Empty callback for incoming messages.

  // A sink that rotates at midnight
  auto sink =
      spdlog::daily_logger_mt("daily_logger", "NMEA0183/sentences.txt", 0, 0);
  sink->set_pattern("%v");

  // Wrap everything in a sentence handler lambda
  auto sentence_handler =
      [&verbose, &sink](
          const std::string &sentence,
          const std::chrono::system_clock::time_point &timestamp) {
        // TODO(freol35241): Publish message on OpenDaVINCI session

        // Log to disk
        auto ms_since_epoch =
            std::chrono::duration_cast<std::chrono::microseconds>(
                timestamp.time_since_epoch())
                .count();
        std::stringstream message;
        message << ms_since_epoch << " " << sentence;
        sink->info(message.str());
        sink->flush();

        if (verbose) {
          std::cout << message.str() << std::endl;
        }
      };

  // And finally use this in a SentenceAssembler
  NMEA0183SentenceAssembler assembler(sentence_handler);

  // Setup a connection to an UDP source with incoming NMEA0183
  // messages
  cluon::UDPReceiver connection{
      address, port,
      [&assembler](std::string &&d, std::string && /*from*/,
                   std::chrono::system_clock::time_point &&tp) noexcept {
        assembler(d, std::move(tp));
      }};

  using namespace std::literals::chrono_literals;  // NOLINT
  while (connection.isRunning()) {
    std::this_thread::sleep_for(1s);
  }
}

auto main(int argc, char **argv) -> int {
  CLI::App app{"NMEA0183 UDP Eavesdropper"};

  // General options for this service
  uint16_t cid = 111;
  app.add_option("-c,--cid", cid, "OpenDaVINCI session id");
  uint16_t id = 1;
  app.add_option("-i,--id", id, "Identification id of this microservice");
  bool verbose = false;
  app.add_flag("--verbose", verbose, "Print to cout");

  app.require_subcommand(1);

  // Gather subcommand
  auto gather = app.add_subcommand(
      "gather",
      "Run in data gathering mode, i.e connect to a UDP stream, listen for "
      "incoming NMEA0183 messages and publish them on an OD4 session");
  std::string address;
  gather->add_option("-a,--address", address, "IP address of stream")
      ->required();
  uint16_t port;
  gather->add_option("-p,--port", port, "Port number to connect to")
      ->required();
  gather->callback(
      [&]() { return run_as_gatherer(cid, id, address, port, verbose); });

  CLI11_PARSE(app, argc, argv);

  return 0;
}
