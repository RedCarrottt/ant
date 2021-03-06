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

#ifndef __ANT_GATEWAY_DFE_INTERNAL_H__
#define __ANT_GATEWAY_DFE_INTERNAL_H__

void interpreters_destroy(void *interpreters);

// TODO(RedCarrottt): hard-coding input
void *ant_gateway_dfeLoadAndPreprocessImage_internal(const char *imgPath);
void *ant_gateway_dfeLoadAndPreprocessImage_getOutputBufferWithLength(
    void *inputTensor, size_t *pInputTensorLength);
void ant_gateway_dfeLoadAndPreprocessImage_releaseOutput(void *inputTensor);

void *ant_gateway_dfeLoad_internal(const char *modelName, int numFragments);
void *ant_gateway_dfeExecute_internal(void *interpreters, void *inputTensor,
                                      size_t inputTensorLength,
                                      int startLayerNum, int endLayerNum);
void *
ant_gateway_dfeExecute_getOutputBufferWithLength(void *outputTensor,
                                                 size_t *pOutputTensorLength);
void ant_gateway_dfeExecute_releaseOutput(void *outputTensor);

#endif /* !defined(__ANT_GATEWAY_DFE_INTERNAL_H__) */
