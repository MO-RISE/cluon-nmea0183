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
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <chrono>  // NOLINT
#include <string>
#include <vector>

#include "assembler.hpp"

TEST_CASE("Test Assembler with empty payload.") {
  std::string output;

  std::string DATA;

  Assembler a{[&output](std::string&& line,
                        std::chrono::system_clock::time_point&& timestamp) {
    output = std::move(line);
  }};
  a(std::move(DATA), std::chrono::system_clock::time_point());

  REQUIRE(output.empty());
}

TEST_CASE("Test Assembler with single correct sentence") {
  size_t call_count{0};
  std::string output;

  std::string DATA{
      "$GNGGA,122144.75,5741.1528,N,01153.1746,E,4,-1,,3.29,M,35.78,M,,*"
      "4D\r\n"};

  Assembler a{[&output, &call_count](
                  std::string&& line,
                  std::chrono::system_clock::time_point&& timestamp) {
    output = line;
    call_count++;
  }};
  a(std::move(DATA), std::chrono::system_clock::time_point());

  REQUIRE_EQ(output, DATA.substr(0, DATA.size() - 2));
  REQUIRE_EQ(call_count, 1);
}

TEST_CASE("Test Assembler with multiple sentences") {
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

  Assembler a{[&output, &call_count](
                  std::string&& line,
                  std::chrono::system_clock::time_point&& timestamp) {
    output.push_back(line);
    call_count++;
  }};
  a(std::move(DATA1), std::chrono::system_clock::time_point());
  a(std::move(DATA2), std::chrono::system_clock::time_point());
  a(std::move(DATA3), std::chrono::system_clock::time_point());

  REQUIRE_EQ(call_count, 3);
  REQUIRE_EQ(output[0], DATA1.substr(0, DATA1.size() - 2));
  REQUIRE_EQ(output[1], DATA2.substr(0, DATA2.size() - 2));
  REQUIRE_EQ(output[2], DATA3.substr(0, DATA3.size() - 2));
}

TEST_CASE("Test Assembler with split sentence") {
  size_t call_count{0};
  std::string output;

  std::string DATA{
      "$GNGGA,122144.75,5741.1528,N,01153.1746,E,4,-1,,3.29,"
      "M,35.78,M,,*4D\r\n"};

  std::string input =
      DATA.substr(23, DATA.size()) + DATA + DATA + DATA.substr(0, 29);

  Assembler a{[&output, &call_count](
                  std::string&& line,
                  std::chrono::system_clock::time_point&& timestamp) {
    output = line;
    call_count++;
  }};
  a(std::move(input), std::chrono::system_clock::time_point());

  REQUIRE_EQ(output, DATA.substr(0, DATA.size() - 2));
  REQUIRE_EQ(call_count, 3);
}
