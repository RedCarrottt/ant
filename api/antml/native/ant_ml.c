/* Copyright (c) 2017-2021 SKKU ESLAB, and contributors. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iotjs_def.h>
#include <iotjs_uv_request.h>
#include <modules/iotjs_module_buffer.h>

#include "../../common/native/ant_common.h"
#include "./internal/ant_ml_internal.h"

JS_FUNCTION(ant_ml_getMaxOfBuffer) {
  jerry_value_t argBuffer;
  iotjs_string_t argType;

  const char *type;
  iotjs_bufferwrap_t *buffer_wrap;
  size_t buffer_len;

  DJS_CHECK_ARGS(2, object, string);
  argBuffer = JS_GET_ARG(0, object);
  argType = JS_GET_ARG(1, string);

  type = iotjs_string_data(&argType);
  buffer_wrap = iotjs_jbuffer_get_bufferwrap_ptr(argBuffer);
  if (buffer_wrap == NULL) {
    return JS_CREATE_ERROR(TYPE, "Invalid buffer given");
  }
  buffer_len = iotjs_bufferwrap_length(buffer_wrap);

  if (buffer_len <= 0) {
    fprintf(stderr, "Invalid buffer length!: %d\n", buffer_len);
    iotjs_string_destroy(&argType);
    return jerry_create_undefined();
  }

  if (strncmp(type, "uint8", strlen("uint8")) == 0) {
    unsigned char *data_array = (unsigned char *)buffer_wrap->buffer;
    size_t data_len = buffer_len / sizeof(unsigned char);
    int result_max_index;
    unsigned char result_value;
    jerry_value_t ret;

    ant_ml_getMaxOfBuffer_internal_uint8(data_array, data_len,
                                         &result_max_index, &result_value);

    ret = jerry_create_object();
    iotjs_jval_set_property_jval(ret, "max_value",
                                 jerry_create_number(result_value));
    iotjs_jval_set_property_jval(ret, "max_index",
                                 jerry_create_number(result_max_index));
    iotjs_string_destroy(&argType);
    return ret;
  } else if (strncmp(type, "int32", strlen("int32")) == 0) {
    int32_t *data_array = (int32_t *)buffer_wrap->buffer;
    size_t data_len = buffer_len / sizeof(int32_t);
    int result_max_index;
    int32_t result_value;
    jerry_value_t ret;

    ant_ml_getMaxOfBuffer_internal_int32(data_array, data_len,
                                         &result_max_index, &result_value);

    ret = jerry_create_object();
    iotjs_jval_set_property_jval(ret, "max_value",
                                 jerry_create_number(result_value));
    iotjs_jval_set_property_jval(ret, "max_index",
                                 jerry_create_number(result_max_index));
    iotjs_string_destroy(&argType);
    return ret;
  } else if (strncmp(type, "float32", strlen("float32")) == 0) {
    float *data_array = (float *)buffer_wrap->buffer;
    size_t data_len = buffer_len / sizeof(float);
    int result_max_index;
    float result_value;
    jerry_value_t ret;

    ant_ml_getMaxOfBuffer_internal_float32(data_array, data_len,
                                           &result_max_index, &result_value);

    ret = jerry_create_object();
    iotjs_jval_set_property_jval(ret, "max_value",
                                 jerry_create_number(result_value));
    iotjs_jval_set_property_jval(ret, "max_index",
                                 jerry_create_number(result_max_index));
    iotjs_string_destroy(&argType);
    return ret;
  } else {
    fprintf(stderr, "Invalid type for getMaxOfBuffer!: %s\n", type);
    fprintf(stderr, "  - Valid types: uint8, int32, float32\n");
    iotjs_string_destroy(&argType);
    return jerry_create_undefined();
  }
}

JS_FUNCTION(ant_ml_toFloatArray) {
  jerry_value_t argBuffer;
  DJS_CHECK_ARGS(1, object);
  argBuffer = JS_GET_ARG(0, object);

  iotjs_bufferwrap_t *buffer_wrap;
  buffer_wrap = iotjs_jbuffer_get_bufferwrap_ptr(argBuffer);

  float *data_array = (float *)buffer_wrap->buffer;
  size_t buffer_len = iotjs_bufferwrap_length(buffer_wrap);
  size_t data_array_len = buffer_len / sizeof(float);

  jerry_value_t retArray = jerry_create_object();
  for (int i = 0; i < (int)data_array_len; i++) {
    jerry_value_t element = jerry_create_number(data_array[i]);
    iotjs_jval_set_property_by_index(retArray, (uint32_t)i, element);
  }

  return retArray;
}

jerry_value_t InitANTML() {
  jerry_value_t nativeObj = jerry_create_object();
  REGISTER_ANT_API(nativeObj, ant_ml, getMaxOfBuffer);
  REGISTER_ANT_API(nativeObj, ant_ml, toFloatArray);

  return nativeObj;
}
