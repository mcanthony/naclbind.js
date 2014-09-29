// Copyright 2014 Ben Smith. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "test_gen.h"

#define REQUEST_JSON(fn, value) \
    "{\"id\": 1," \
    " \"set\": {\"1\": " #value "}," \
    " \"commands\": [{\"id\": " #fn ", \"args\": [1], \"ret\": 2}]," \
    " \"get\": [2]," \
    " \"destroy\": [1, 2]" \
    "}"

#define RESPONSE_JSON(value) \
    "{\"id\":1,\"values\":[" #value "]}\n"

TEST_F(GeneratorTest, Char) {
  RunTest(REQUEST_JSON(1, 42), RESPONSE_JSON(42));
}

TEST_F(GeneratorTest, Schar) {
  RunTest(REQUEST_JSON(2, -42), RESPONSE_JSON(-42));
}

TEST_F(GeneratorTest, Uchar) {
  RunTest(REQUEST_JSON(3, 142), RESPONSE_JSON(142));
}

TEST_F(GeneratorTest, Short) {
  RunTest(REQUEST_JSON(4, -4200), RESPONSE_JSON(-4200));
}

TEST_F(GeneratorTest, Ushort) {
  RunTest(REQUEST_JSON(5, 14200), RESPONSE_JSON(14200));
}

TEST_F(GeneratorTest, Int) {
  RunTest(REQUEST_JSON(6, -420000), RESPONSE_JSON(-420000));
}

TEST_F(GeneratorTest, Uint) {
  RunTest(REQUEST_JSON(7, 1420000), RESPONSE_JSON(1420000));
}

TEST_F(GeneratorTest, Long) {
  RunTest(REQUEST_JSON(8, -420000), RESPONSE_JSON(-420000));
}

TEST_F(GeneratorTest, Ulong) {
  RunTest(REQUEST_JSON(9, 1420000), RESPONSE_JSON(1420000));
}

// TODO(binji): int64 values are not yet implemented
TEST_F(GeneratorTest, DISABLED_LongLong) {
  RunTest(REQUEST_JSON(10, -420000), RESPONSE_JSON(-420000));
}

// TODO(binji): int64 values are not yet implemented
TEST_F(GeneratorTest, DISABLED_UlongLong) {
  RunTest(REQUEST_JSON(11, 1420000), RESPONSE_JSON(1420000));
}

TEST_F(GeneratorTest, Float) {
  RunTest(REQUEST_JSON(12, 3.5), RESPONSE_JSON(3.50));
}

TEST_F(GeneratorTest, Double) {
  RunTest(REQUEST_JSON(13, 1e13), RESPONSE_JSON(10000000000000.0));
}
