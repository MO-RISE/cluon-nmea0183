/// Copyright 2021 RISE Research Institute of Sweden. All rights reserved.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <chrono>  // NOLINT
#include <string>
#include <vector>

#include "NMEA0183_assembler.hpp"

TEST_CASE("Test NMEA0183SentenceAssembler with empty payload.") {
  std::string output;

  std::string DATA;

  NMEA0183SentenceAssembler a{
      [&output](const std::string& line,
                const std::chrono::system_clock::time_point& timestamp) {
        output = line;
      }};
  a(DATA, std::chrono::system_clock::time_point());

  REQUIRE(output.empty());
}

TEST_CASE("Test NMEA0183SentenceAssembler with payload of wrong format") {
  size_t call_count{0};
  std::string output;

  std::string DATA{"skfvdalfdadnsldasdc\nsakhbsacdsad\nahsdvds\nadjd"};

  NMEA0183SentenceAssembler a{
      [&output, &call_count](
          const std::string& line,
          const std::chrono::system_clock::time_point& timestamp) {
        output = line;
        call_count++;
      }};
  a(DATA, std::chrono::system_clock::time_point());

  REQUIRE(output.empty());
  REQUIRE_EQ(call_count, 0);
}

TEST_CASE("Test NMEA0183SentenceAssembler with single correct sentence") {
  size_t call_count{0};
  std::string output;

  std::string DATA{
      "$GNGGA,122144.75,5741.1528,N,01153.1746,E,4,-1,,3.29,M,35.78,M,,*"
      "4D\r\n"};

  NMEA0183SentenceAssembler a{
      [&output, &call_count](
          const std::string& line,
          const std::chrono::system_clock::time_point& timestamp) {
        output = line;
        call_count++;
      }};
  a(DATA, std::chrono::system_clock::time_point());

  REQUIRE_EQ(output, DATA.substr(0, DATA.size() - 2));
  REQUIRE_EQ(call_count, 1);
}

TEST_CASE(
    "Test NMEA0183SentenceAssembler with single correct sentence inbetween "
    "scramble") {
  size_t call_count{0};
  std::string output;

  std::string DATA{
      "$GNGGA,122144.75,5741.1528,N,01153.1746,E,4,-1,,3.29,M,35.78,M,,*"
      "4D\r\n"};

  std::string scramble{"dadnsldasdc\n"};

  NMEA0183SentenceAssembler a{
      [&output, &call_count](
          const std::string& line,
          const std::chrono::system_clock::time_point& timestamp) {
        output = line;
        call_count++;
      }};
  a(scramble + DATA + scramble, std::chrono::system_clock::time_point());

  REQUIRE_EQ(output, DATA.substr(0, DATA.size() - 2));
  REQUIRE_EQ(call_count, 1);
}

TEST_CASE("Test NMEA0183SentenceAssembler with multiple sentences") {
  size_t call_count{0};
  std::vector<std::string> output;

  std::string DATA1{
      "$GNGGA,122144.75,5741.1528,N,01153.1746,E,4,-1,,3.29,M,35.78,M,,*"
      "4D\r\n"};
  std::string DATA2{
      "$GBGSV,12,12,07,422,39,272,46,07,427,23,114,46,07,428,27,057,45,,,,,*"
      "45\r\n"};
  std::string DATA3{
      "$GNGGA,122144.75,5741.1528,N,01153.1746,E,4,-1,,3.29,M,35.78,M,,*"
      "4D\r\n"};

  NMEA0183SentenceAssembler a{
      [&output, &call_count](
          const std::string& line,
          const std::chrono::system_clock::time_point& timestamp) {
        output.push_back(line);
        call_count++;
      }};
  a(DATA1, std::chrono::system_clock::time_point());
  a(DATA2, std::chrono::system_clock::time_point());
  a(DATA3, std::chrono::system_clock::time_point());

  REQUIRE_EQ(call_count, 3);
  REQUIRE_EQ(output[0], DATA1.substr(0, DATA1.size() - 2));
  REQUIRE_EQ(output[1], DATA2.substr(0, DATA2.size() - 2));
  REQUIRE_EQ(output[2], DATA3.substr(0, DATA3.size() - 2));
}

TEST_CASE("Test NMEA0183SentenceAssembler with split sentence") {
  size_t call_count{0};
  std::string output;

  std::string DATA{
      "$GNGGA,122144.75,5741.1528,N,01153.1746,E,4,-1,,3.29,"
      "M,35.78,M,,*4D\r\n"};

  std::string input =
      DATA.substr(23, DATA.size()) + DATA + DATA + DATA.substr(0, 29);

  NMEA0183SentenceAssembler a{
      [&output, &call_count](
          const std::string& line,
          const std::chrono::system_clock::time_point& timestamp) {
        output = line;
        call_count++;
      }};
  a(input, std::chrono::system_clock::time_point());

  REQUIRE_EQ(output, DATA.substr(0, DATA.size() - 2));
  REQUIRE_EQ(call_count, 2);
}
