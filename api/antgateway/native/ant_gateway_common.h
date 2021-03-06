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

#ifndef __ANT_GATEWAY_COMMON_H__
#define __ANT_GATEWAY_COMMON_H__

#define JS_DECLARE_PTR2(JOBJ, TYPE, NAME, TYPE2)                               \
  TYPE *NAME = NULL;                                                           \
  do {                                                                         \
    if (!jerry_get_object_native_pointer(JOBJ, (void **)&NAME,                 \
                                         &TYPE2##_native_info)) {              \
      return JS_CREATE_ERROR(COMMON, "Internal");                              \
    }                                                                          \
  } while (0)

#endif /* !defined(__ANT_GATEWAY_COMMON_H__) */
