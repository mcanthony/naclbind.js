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

#include <stdlib.h>
#include <vector>
#include <gtest/gtest.h>
#include <ppapi/c/pp_var.h>
#include "fake_interfaces.h"
#include "error.h"
#include "handle.h"
#include "var.h"

bool operator==(struct PP_Var v1, struct PP_Var v2) {
  if (v1.type != v2.type) {
    return false;
  }

  switch (v1.type) {
    case PP_VARTYPE_UNDEFINED:
    case PP_VARTYPE_NULL:
      return NB_TRUE;

    case PP_VARTYPE_BOOL:
      return v1.value.as_bool == v2.value.as_bool;

    case PP_VARTYPE_INT32:
      return v1.value.as_int == v2.value.as_int;

    case PP_VARTYPE_DOUBLE:
      return v1.value.as_double == v2.value.as_double;

    case PP_VARTYPE_STRING:
    case PP_VARTYPE_OBJECT:
    case PP_VARTYPE_ARRAY:
    case PP_VARTYPE_DICTIONARY:
    case PP_VARTYPE_ARRAY_BUFFER:
    case PP_VARTYPE_RESOURCE:
      return v1.value.as_id == v2.value.as_id;

    default:
      return false;
  }
}

bool operator==(struct PP_Var v, const char* s1) {
  if (v.type != PP_VARTYPE_STRING) {
    return false;
  }

  const char* s2;
  uint32_t len2;
  if (!nb_var_string(v, &s2, &len2)) {
    return false;
  }

  uint32_t len1 = strlen(s1);
  if (len1 != len2) {
    return false;
  }

  return strncmp(s1, s2, len1) == 0;
}

bool operator==(const char* s, struct PP_Var v) {
  return v == s;
}

bool operator==(struct PP_Var v, void* p) {
  switch (v.type) {
    case PP_VARTYPE_STRING:
      return v == (const char*)p;

    case PP_VARTYPE_NULL:
      return p == NULL;

    default:
      return false;
  }
}

bool operator==(void* p, struct PP_Var v) {
  return v == p;
}

class HandleTest : public ::testing::Test {
 public:
  HandleTest() {}

  virtual void SetUp() { EXPECT_EQ(0, nb_handle_count()); }

  virtual void TearDown() {
    EXPECT_EQ(NB_TRUE, fake_interface_check_no_references());
    EXPECT_EQ(0, nb_handle_count());
  }
};

#define EXPECT_GET(type, suffix, handle, expected)                         \
  type val_##suffix;                                                       \
  NB_Bool result_##suffix = nb_handle_get_##suffix(handle, &val_##suffix); \
  EXPECT_EQ(NB_TRUE, result_##suffix);                                     \
  if (result_##suffix) {                                                   \
    EXPECT_EQ(expected, val_##suffix);                                     \
  }

#define EXPECT_FAIL(type, suffix, handle) \
  type val_##suffix;                      \
  EXPECT_EQ(NB_FALSE, nb_handle_get_##suffix(handle, &val_##suffix))

#define EXPECT_O(type, suffix, handle, expected) \
  EXPECT_GET(type, suffix, handle, expected)

#define EXPECT_T(type, suffix, handle, expected) \
  type val_##suffix;                             \
  EXPECT_EQ(NB_TRUE, nb_handle_get_##suffix(handle, &val_##suffix));

#define EXPECT__(type, suffix, handle, expected) \
  EXPECT_FAIL(type, suffix, handle)

#define ROW(reg, val, i8, u8, i16, u16, i32, u32, i64, u64, f32, f64, vp, v) \
  do {                                                                       \
    ASSERT_EQ(NB_TRUE, nb_handle_register_##reg(1, val));                    \
    EXPECT_##i8(int8_t, int8, 1, val);                                       \
    EXPECT_##u8(uint8_t, uint8, 1, val);                                     \
    EXPECT_##i16(int16_t, int16, 1, val);                                    \
    EXPECT_##u16(uint16_t, uint16, 1, val);                                  \
    EXPECT_##i32(int32_t, int32, 1, val);                                    \
    EXPECT_##u32(uint32_t, uint32, 1, val);                                  \
    EXPECT_##i64(int64_t, int64, 1, val);                                    \
    EXPECT_##u64(uint64_t, uint64, 1, val);                                  \
    EXPECT_##f32(float, float, 1, val);                                      \
    EXPECT_##f64(double, double, 1, val);                                    \
    EXPECT_##vp(void*, voidp, 1, val);                                       \
    EXPECT_##v(struct PP_Var, var, 1, val);                                  \
    nb_handle_destroy(1);                                                    \
  } while (0)

TEST_F(HandleTest, CantRegisterTwice) {
  EXPECT_EQ(NB_TRUE, nb_handle_register_int32(1, 42));
  EXPECT_EQ(NB_FALSE, nb_handle_register_int8(1, 42));

  nb_handle_destroy(1);
}

static void dummy_func(void) {}

TEST_F(HandleTest, Basic) {
  struct PP_Var var;
  void* voidp = &var;
  void (*funcp)(void) = &dummy_func;

  var = nb_var_string_create("hello", 5);

  EXPECT_EQ(NB_TRUE, nb_handle_register_int8(1, -42));
  EXPECT_EQ(NB_TRUE, nb_handle_register_uint8(2, 42));
  EXPECT_EQ(NB_TRUE, nb_handle_register_int16(3, -420));
  EXPECT_EQ(NB_TRUE, nb_handle_register_uint16(4, 420));
  EXPECT_EQ(NB_TRUE, nb_handle_register_int32(5, -420000L));
  EXPECT_EQ(NB_TRUE, nb_handle_register_uint32(6, 420000UL));
  EXPECT_EQ(NB_TRUE, nb_handle_register_int64(7, -42000000000LL));
  EXPECT_EQ(NB_TRUE, nb_handle_register_uint64(8, 42000000000ULL));
  EXPECT_EQ(NB_TRUE, nb_handle_register_float(9, 3.25));
  EXPECT_EQ(NB_TRUE, nb_handle_register_double(10, 1e30));
  EXPECT_EQ(NB_TRUE, nb_handle_register_voidp(11, voidp));
  EXPECT_EQ(NB_TRUE, nb_handle_register_funcp(12, funcp));
  EXPECT_EQ(NB_TRUE, nb_handle_register_var(13, var));

  EXPECT_GET(int8_t, int8, 1, -42);
  EXPECT_GET(uint8_t, uint8, 2, 42);
  EXPECT_GET(int16_t, int16, 3, -420);
  EXPECT_GET(uint16_t, uint16, 4, 420);
  EXPECT_GET(int32_t, int32, 5, -420000L);
  EXPECT_GET(uint32_t, uint32, 6, 420000UL);
  EXPECT_GET(int64_t, int64, 7, -42000000000LL);
  EXPECT_GET(uint64_t, uint64, 8, 42000000000ULL);
  EXPECT_GET(float, float, 9, 3.25);
  EXPECT_GET(double, double, 10, 1e30);
  EXPECT_GET(void*, voidp, 11, voidp);

  void (*val_funcp)(void);
  EXPECT_EQ(NB_TRUE, nb_handle_get_funcp(12, &val_funcp));
  EXPECT_EQ(val_funcp, funcp);

  struct PP_Var val_var;
  EXPECT_EQ(NB_TRUE, nb_handle_get_var(13, &val_var));
  EXPECT_EQ(PP_VARTYPE_STRING, val_var.type);
  EXPECT_EQ(val_var.value.as_id, var.value.as_id);

  nb_var_release(var);

  NB_Handle to_destroy[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
  nb_handle_destroy_many(&to_destroy[0], 13);
}

TEST_F(HandleTest, Int8) {
  //               i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(int8, -0x70,  O, _,  O,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int8,     0,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int8, +0x70,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
}

TEST_F(HandleTest, Uint8) {
  //                i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(uint8,     0,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint8, +0x70,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint8, +0xf0,  _, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
}

TEST_F(HandleTest, Int16) {
  //                  i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(int16, -0x7000,  _, _,  O,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int16,   -0x70,  O, _,  O,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int16,       0,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int16,   +0x70,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int16,   +0xf0,  _, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int16, +0x7000,  _, _,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
}

TEST_F(HandleTest, Uint16) {
  //                   i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(uint16,       0,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint16,   +0x70,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint16,   +0xf0,  _, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint16, +0x7000,  _, _,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint16, +0xf000,  _, _,  _,  O,  O,  O,  O,  O,  O,  O, _, _);
}

TEST_F(HandleTest, Int32) {
  //                      i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(int32, -0x70000000,  _, _,  _,  _,  O,  _,  O,  _,  _,  O, _, _);
  ROW(int32,   -0x700000,  _, _,  _,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int32,     -0x7000,  _, _,  O,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int32,       -0x70,  O, _,  O,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int32,           0,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int32,       +0x70,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int32,       +0xf0,  _, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int32,     +0x7000,  _, _,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int32,     +0xf000,  _, _,  _,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int32,   +0x700000,  _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int32,   +0xf00000,  _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int32, +0x70000000,  _, _,  _,  _,  O,  O,  O,  O,  _,  O, _, _);
}

TEST_F(HandleTest, Uint32) {
  //                       i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(uint32,           0,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint32,       +0x70,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint32,       +0xf0,  _, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint32,     +0x7000,  _, _,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint32,     +0xf000,  _, _,  _,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint32,   +0x700000,  _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint32,   +0xf00000,  _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint32, +0x70000000,  _, _,  _,  _,  O,  O,  O,  O,  _,  O, _, _);
  ROW(uint32, +0xf0000000,  _, _,  _,  _,  _,  O,  O,  O,  _,  O, _, _);
}

TEST_F(HandleTest, Int64) {
  //                               i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(int64, -0x7000000000000000LL, _, _,  _,  _,  _,  _,  O,  _,  _,  _, _, _);
  ROW(int64,    -0x7000000000000LL, _, _,  _,  _,  _,  _,  O,  _,  _,  O, _, _);
  ROW(int64,           -0x70000000, _, _,  _,  _,  O,  _,  O,  _,  _,  O, _, _);
  ROW(int64,             -0x700000, _, _,  _,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int64,               -0x7000, _, _,  O,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int64,                 -0x70, O, _,  O,  _,  O,  _,  O,  _,  O,  O, _, _);
  ROW(int64,                     0, O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int64,                 +0x70, O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int64,                 +0xf0, _, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int64,               +0x7000, _, _,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int64,               +0xf000, _, _,  _,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int64,             +0x700000, _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int64,             +0xf00000, _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(int64,           +0x70000000, _, _,  _,  _,  O,  O,  O,  O,  _,  O, _, _);
  ROW(int64,           +0xf0000000, _, _,  _,  _,  _,  O,  O,  O,  _,  O, _, _);
  ROW(int64,    +0x7000000000000LL, _, _,  _,  _,  _,  _,  O,  O,  _,  O, _, _);
  ROW(int64,    +0xf000000000000LL, _, _,  _,  _,  _,  _,  O,  O,  _,  O, _, _);
  ROW(int64, +0x7000000000000000LL, _, _,  _,  _,  _,  _,  O,  O,  _,  _, _, _);
}

TEST_F(HandleTest, Uint64) {
  //                                i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(uint64,                     0, O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint64,                 +0x70, O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint64,                 +0xf0, _, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint64,               +0x7000, _, _,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint64,               +0xf000, _, _,  _,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint64,             +0x700000, _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint64,             +0xf00000, _, _,  _,  _,  O,  O,  O,  O,  O,  O, _, _);
  ROW(uint64,           +0x70000000, _, _,  _,  _,  O,  O,  O,  O,  _,  O, _, _);
  ROW(uint64,           +0xf0000000, _, _,  _,  _,  _,  O,  O,  O,  _,  O, _, _);
  ROW(uint64,    +0x7000000000000LL, _, _,  _,  _,  _,  _,  O,  O,  _,  O, _, _);
  ROW(uint64,    +0xf000000000000LL, _, _,  _,  _,  _,  _,  O,  O,  _,  O, _, _);
  ROW(uint64, +0x7000000000000000LL, _, _,  _,  _,  _,  _,  O,  O,  _,  _, _, _);
  ROW(uint64, +0xf000000000000000LL, _, _,  _,  _,  _,  _,  _,  O,  _,  _, _, _);
}

TEST_F(HandleTest, Float) {
  //               i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(float,  0.f,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(float,  1.f,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(float, 3.5f,  _, _,  _,  _,  _,  _,  _,  _,  O,  O, _, _);
}

TEST_F(HandleTest, Double) {
  //                i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(double,  0.0,  O, O,  O,  O,  O,  O,  O,  O,  O,  O, _, _);
  ROW(double, 1e11,  _, _,  _,  _,  _,  _,  O,  O,  T,  O, _, _);
  ROW(double, 1e20,  _, _,  _,  _,  _,  _,  _,  _,  T,  O, _, _);
}

TEST_F(HandleTest, Voidp) {
  int dummy;
  void* vp = &dummy;

  //             i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(voidp, vp,  _, _, _,   _,  _,  _,  _,  _,  _,  _, O, _);
}

TEST_F(HandleTest, Funcp) {
  void (*fp)(void) = &dummy_func;

  //             i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
  ROW(funcp, fp,  _, _, _,   _,  _,  _,  _,  _,  _,  _, _, _);
}

TEST_F(HandleTest, Var) {
  EXPECT_EQ(NB_FALSE, nb_handle_register_var(1, PP_MakeUndefined()));
  EXPECT_EQ(NB_FALSE, nb_handle_register_var(1, PP_MakeNull()));
  EXPECT_EQ(NB_FALSE, nb_handle_register_var(1, PP_MakeBool(PP_TRUE)));
  EXPECT_EQ(NB_FALSE, nb_handle_register_var(1, PP_MakeInt32(42)));
  EXPECT_EQ(NB_FALSE, nb_handle_register_var(1, PP_MakeDouble(3.25)));

  {
    struct PP_Var v = nb_var_string_create("hi", 2);
    //           i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
    ROW(var, v,   _, _, _,   _,  _,  _,  _,  _,  _,  _, O, O);
    nb_var_release(v);
  }

  {
    struct PP_Var v = nb_var_array_create();
    //           i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
    ROW(var, v,   _, _, _,   _,  _,  _,  _,  _,  _,  _, _, O);
    nb_var_release(v);
  }

  {
    struct PP_Var v = nb_var_dict_create();
    //           i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
    ROW(var, v,   _, _, _,   _,  _,  _,  _,  _,  _,  _, _, O);
    nb_var_release(v);
  }

  {
    struct PP_Var v = nb_var_buffer_create(10);
    //           i8 u8 i16 u16 i32 u32 i64 u64 f32 f64 vp  v
    ROW(var, v,   _, _, _,   _,  _,  _,  _,  _,  _,  _, _, O);
    nb_var_release(v);
  }
}

TEST_F(HandleTest, Charp) {
  struct PP_Var v = nb_var_string_create("hi", 2);
  ASSERT_EQ(NB_TRUE, nb_handle_register_var(1, v));
  nb_var_release(v);

  char* s;
  EXPECT_EQ(NB_TRUE, nb_handle_get_charp(1, &s));
  EXPECT_STREQ("hi", s);

  void* p;
  EXPECT_EQ(NB_TRUE, nb_handle_get_voidp(1, &p));
  EXPECT_STREQ("hi", (char*)p);

  nb_handle_destroy(1);
}

#define CONVERT_OK(reg, val, pp_type, as)                  \
  {                                                        \
    struct PP_Var var;                                     \
    EXPECT_EQ(NB_TRUE, nb_handle_register_##reg(1, val));  \
    EXPECT_EQ(NB_TRUE, nb_handle_convert_to_var(1, &var)); \
    EXPECT_EQ(pp_type, var.type);                          \
    EXPECT_EQ(val, var.value.as);                          \
    nb_var_release(var);                                   \
    nb_handle_destroy(1);                                  \
  }

#define CONVERT_FAIL(reg, val)                              \
  {                                                         \
    struct PP_Var var;                                      \
    EXPECT_EQ(NB_TRUE, nb_handle_register_##reg(1, val));   \
    EXPECT_EQ(NB_FALSE, nb_handle_convert_to_var(1, &var)); \
    nb_handle_destroy(1);                                   \
  }

TEST_F(HandleTest, ConvertToVar) {
  CONVERT_OK(int8, 0x70, PP_VARTYPE_INT32, as_int);
  CONVERT_OK(uint8, 0xf0, PP_VARTYPE_INT32, as_int);
  CONVERT_OK(int16, 0x7000, PP_VARTYPE_INT32, as_int);
  CONVERT_OK(uint16, 0xf000, PP_VARTYPE_INT32, as_int);
  CONVERT_OK(int32, 0x70000000, PP_VARTYPE_INT32, as_int);
  CONVERT_OK(uint32, 0xf0000000, PP_VARTYPE_INT32, as_int);
  CONVERT_OK(float, 3.25, PP_VARTYPE_DOUBLE, as_double);
  CONVERT_OK(double, 1e11, PP_VARTYPE_DOUBLE, as_double);
  // var
  {
    struct PP_Var var;
    struct PP_Var dummy = nb_var_array_create();
    EXPECT_EQ(NB_TRUE, nb_handle_register_var(1, dummy));
    EXPECT_EQ(NB_TRUE, nb_handle_convert_to_var(1, &var));
    EXPECT_EQ(dummy.type, var.type);
    EXPECT_EQ(dummy.value.as_id, var.value.as_id);
    nb_var_release(var);
    nb_var_release(dummy);
    nb_handle_destroy(1);
  }
  // voidp (with value)
  {
    struct PP_Var var;
    int dummy;
    void* voidp = &dummy;
    EXPECT_EQ(NB_TRUE, nb_handle_register_voidp(1, voidp));
    EXPECT_EQ(NB_TRUE, nb_handle_convert_to_var(1, &var));
    EXPECT_EQ(PP_VARTYPE_INT32, var.type);
    EXPECT_EQ(1, var.value.as_int);  // Returns the handle, of the void*
    nb_var_release(var);
    nb_handle_destroy(1);
  }
  // voidp (NULL)
  {
    struct PP_Var var;
    EXPECT_EQ(NB_TRUE, nb_handle_register_voidp(1, NULL));
    EXPECT_EQ(NB_TRUE, nb_handle_convert_to_var(1, &var));
    EXPECT_EQ(PP_VARTYPE_NULL, var.type);
    nb_var_release(var);
    nb_handle_destroy(1);
  }
  // int64
  {
    struct PP_Var var;
    struct PP_Var tag;
    EXPECT_EQ(NB_TRUE, nb_handle_register_int64(1, 0x10000000000LL));
    EXPECT_EQ(NB_TRUE, nb_handle_convert_to_var(1, &var));
    EXPECT_EQ(PP_VARTYPE_ARRAY, var.type);
    EXPECT_EQ(3, nb_var_array_length(var));
    tag = nb_var_array_get(var, 0);
    EXPECT_EQ(PP_VARTYPE_STRING, tag.type);
    EXPECT_EQ("long", tag);
    EXPECT_EQ(PP_VARTYPE_INT32, nb_var_array_get(var, 1).type);
    EXPECT_EQ(PP_VARTYPE_INT32, nb_var_array_get(var, 2).type);
    EXPECT_EQ(0, nb_var_array_get(var, 1).value.as_int);
    EXPECT_EQ(256, nb_var_array_get(var, 2).value.as_int);
    nb_var_release(tag);
    nb_var_release(var);
    nb_handle_destroy(1);
  }
  // uint64
  {
    struct PP_Var var;
    struct PP_Var tag;
    EXPECT_EQ(NB_TRUE, nb_handle_register_uint64(1, 0xf00000000000000fLL));
    EXPECT_EQ(NB_TRUE, nb_handle_convert_to_var(1, &var));
    EXPECT_EQ(PP_VARTYPE_ARRAY, var.type);
    EXPECT_EQ(3, nb_var_array_length(var));
    tag = nb_var_array_get(var, 0);
    EXPECT_EQ(PP_VARTYPE_STRING, tag.type);
    EXPECT_EQ("long", tag);
    EXPECT_EQ(PP_VARTYPE_INT32, nb_var_array_get(var, 1).type);
    EXPECT_EQ(PP_VARTYPE_INT32, nb_var_array_get(var, 2).type);
    EXPECT_EQ(15, nb_var_array_get(var, 1).value.as_int);
    EXPECT_EQ(-0x10000000, nb_var_array_get(var, 2).value.as_int);
    nb_var_release(tag);
    nb_var_release(var);
    nb_handle_destroy(1);
  }
}

class HandleStressTest : public HandleTest {};

TEST_F(HandleStressTest, Basic) {
  const int cycles = 100;
  const int create_per_cycle = 1000;
  const int destroy_per_cycle = 800;
  unsigned int seed = 0xface;
  NB_Handle next_handle = 1;
  std::vector<NB_Handle> to_destroy;
  for (int i = 0; i < cycles; ++i) {
    for (int j = 0; j < create_per_cycle; ++j) {
      NB_Handle handle = next_handle++;
      ASSERT_EQ(NB_TRUE, nb_handle_register_int32(handle, handle));
      to_destroy.push_back(handle);
    }

    for(int j = 0; j < destroy_per_cycle; ++j) {
      int index = rand_r(&seed) % to_destroy.size();
      NB_Handle handle = to_destroy[index];
      int32_t val;
      ASSERT_EQ(NB_TRUE, nb_handle_get_int32(handle, &val));
      ASSERT_EQ(handle, val);
      nb_handle_destroy(handle);
      std::swap(to_destroy[index], to_destroy.back());
      to_destroy.pop_back();
    }
  }

  nb_handle_destroy_many(to_destroy.data(), to_destroy.size());
}

TEST_F(HandleStressTest, OneHandle) {
  const int cycles = 1000;
  for (int i = 0; i < cycles; ++i) {
    ASSERT_EQ(NB_TRUE, nb_handle_register_int32(1, 42));
    int32_t val;
    ASSERT_EQ(NB_TRUE, nb_handle_get_int32(1, &val));
    ASSERT_EQ(42, val);
    nb_handle_destroy(1);
  }
}
