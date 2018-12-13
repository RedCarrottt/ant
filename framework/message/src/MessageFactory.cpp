/* Copyright (c) 2017 SKKU ESLAB, and contributors. All rights reserved.
 *
 * Contributor: Gyeonghwan Hong<redcarrottt@gmail.com>
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

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ANTdbugLog.h"
#include "AppCoreMessage.h"
#include "AppMessage.h"
#include "BaseMessage.h"
#include "CompanionMessage.h"
#include "MessageFactory.h"
#include "cJSON.h"

#define RETURN_IF_INVALID_CJSON_OBJ(a, ret)                                    \
  if ((a) == NULL) {                                                           \
    ANT_DBG_ERR("JSON handling error: %s", cJSON_GetErrorPtr());               \
    return ret;                                                                \
  }

int MessageFactory::currentMessageId = 0;

BaseMessage *MessageFactory::makeMessageFromJSONString(const char *rawString) {
  // Parse rawString in JSON
  cJSON *thisObj = cJSON_Parse(rawString);
  RETURN_IF_INVALID_CJSON_OBJ(thisObj, NULL);
  return makeBaseMessageFromJSON(thisObj);
}

BaseMessage *MessageFactory::makeBaseMessageFromJSON(cJSON *messageObj) {
  cJSON *thisObj = messageObj;

  // Get properties
  // messageId
  cJSON *messageIdObj =
      cJSON_GetObjectItem(thisObj, BASE_MESSAGE_KEY_MESSAGE_NUM);
  RETURN_IF_INVALID_CJSON_OBJ(messageIdObj, NULL);
  int messageId = atoi(messageIdObj->valuestring);

  // Sender URI
  cJSON *senderUriObj =
      cJSON_GetObjectItem(thisObj, BASE_MESSAGE_KEY_SENDER_URI);
  RETURN_IF_INVALID_CJSON_OBJ(senderUriObj, NULL);
  std::string senderUri(senderUriObj->valuestring);

  // URI
  cJSON *uriObj = cJSON_GetObjectItem(thisObj, BASE_MESSAGE_KEY_URI);
  RETURN_IF_INVALID_CJSON_OBJ(uriObj, NULL);
  std::string uri(uriObj->valuestring);

  // Type
  cJSON *typeObj = cJSON_GetObjectItem(thisObj, BASE_MESSAGE_KEY_TYPE);
  RETURN_IF_INVALID_CJSON_OBJ(typeObj, NULL);
  // BE AWARE: Since cJSON internally consider integer value as Number(double),
  // I decided to use string-shaped integer in JSON.
  // Once integer value is carried by Number variable, its value can be changed
  // because there is a limit in mantissa's representation.
  BaseMessageType::Value type =
      static_cast<BaseMessageType::Value>(atoi(typeObj->valuestring));

  // isFileAttached
  cJSON *isFileAttachedObj =
      cJSON_GetObjectItem(thisObj, BASE_MESSAGE_KEY_IS_FILE_ATTACHED);
  RETURN_IF_INVALID_CJSON_OBJ(isFileAttachedObj, NULL);
  int isFileAttachedNum = atoi(isFileAttachedObj->valuestring);
  bool isFileAttached = (isFileAttachedNum == 0) ? false : true;

  // FileName
  std::string fileName("");
  if (isFileAttachedNum) {
    cJSON *fileNameObj =
        cJSON_GetObjectItem(thisObj, BASE_MESSAGE_KEY_FILE_NAME);
    RETURN_IF_INVALID_CJSON_OBJ(fileNameObj, NULL);
    fileName = fileNameObj->valuestring;
  }

  cJSON *payloadObj = cJSON_GetObjectItem(thisObj, BASE_MESSAGE_KEY_PAYLOAD);
  RETURN_IF_INVALID_CJSON_OBJ(payloadObj, NULL);

  // Allocate and initialize a new BaseMessage
  BaseMessage *newMessage = new BaseMessage(messageId, senderUri, uri, type,
                                            isFileAttached, fileName);
  BaseMessagePayload *messagePayload = NULL;
  switch (type) {
  case BaseMessageType::AppCore:
    messagePayload = makeAppCoreMessageFromJSON(payloadObj);
    break;
  case BaseMessageType::App:
    messagePayload = makeAppMessageFromJSON(payloadObj);
    break;
  case BaseMessageType::AppAck:
    messagePayload = makeAppAckMessageFromJSON(payloadObj);
    break;
  case BaseMessageType::AppCoreAck:
    messagePayload = makeAppCoreAckMessageFromJSON(payloadObj);
    break;
  case BaseMessageType::Companion:
    messagePayload = makeCompanionMessageFromJSON(payloadObj);
    break;
  case BaseMessageType::ML:
    messagePayload = makeMLMessageFromJSON(payloadObj);
    break;
  case BaseMessageType::MLAck:
    messagePayload = makeMLAckMessageFromJSON(payloadObj);
    break;
  case BaseMessageType::ResourceRequest:
    messagePayload = makeResourceRequestFromJSON(payloadObj);
    break;
  case BaseMessageType::ResourceResponse:
    messagePayload = makeResourceResponseFromJSON(payloadObj);
    break;
  default:
    // These types cannot be handled.
    break;
  }
  newMessage->setPayload(messagePayload);
  return newMessage;
}

AppCoreMessage *
MessageFactory::makeAppCoreMessageFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *commandTypeObj =
      cJSON_GetObjectItem(thisObj, APPCORE_MESSAGE_KEY_COMMAND_TYPE);
  RETURN_IF_INVALID_CJSON_OBJ(commandTypeObj, NULL);
  AppCoreMessageCommandType::Value commandType =
      static_cast<AppCoreMessageCommandType::Value>(
          atoi(commandTypeObj->valuestring));

  cJSON *appCorePayloadObj =
      cJSON_GetObjectItem(thisObj, APPCORE_MESSAGE_KEY_PAYLOAD);

  // Allocate and initialize a new AppCoreMessage
  AppCoreMessage *newMessage = new AppCoreMessage(commandType);
  if (appCorePayloadObj != NULL) {
    newMessage->setAppCorePayloadObj(appCorePayloadObj);
  }
  return newMessage;
}

AppCoreAckMessage *
MessageFactory::makeAppCoreAckMessageFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *commandMessageIdObj = cJSON_GetObjectItem(thisObj, "commandMessageId");
  RETURN_IF_INVALID_CJSON_OBJ(commandMessageIdObj, NULL);
  int commandMessageId = atoi(commandMessageIdObj->valuestring);

  cJSON *commandTypeObj = cJSON_GetObjectItem(thisObj, "commandType");
  RETURN_IF_INVALID_CJSON_OBJ(commandTypeObj, NULL);
  AppCoreMessageCommandType::Value commandType =
      static_cast<AppCoreMessageCommandType::Value>(
          atoi(commandTypeObj->valuestring));

  cJSON *appCoreAckPayloadObj = cJSON_GetObjectItem(thisObj, "payload");

  // Allocate and initialize a new AppAckMessage
  AppCoreAckMessage *newMessage =
      new AppCoreAckMessage(commandMessageId, commandType);
  if (appCoreAckPayloadObj != NULL) {
    newMessage->setAppCoreAckPayloadObj(appCoreAckPayloadObj);
  }
  return newMessage;
}

AppAckMessage *
MessageFactory::makeAppAckMessageFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *commandMessageIdObj = cJSON_GetObjectItem(thisObj, "commandMessageId");
  RETURN_IF_INVALID_CJSON_OBJ(commandMessageIdObj, NULL);
  int commandMessageId = atoi(commandMessageIdObj->valuestring);

  cJSON *commandTypeObj = cJSON_GetObjectItem(thisObj, "commandType");
  RETURN_IF_INVALID_CJSON_OBJ(commandTypeObj, NULL);
  AppMessageCommandType::Value commandType =
      static_cast<AppMessageCommandType::Value>(
          atoi(commandTypeObj->valuestring));

  cJSON *appAckPayloadObj = cJSON_GetObjectItem(thisObj, "payload");

  // Allocate and initialize a new AppAckMessage
  AppAckMessage *newMessage = new AppAckMessage(commandMessageId, commandType);
  if (appAckPayloadObj != NULL) {
    newMessage->setAppAckPayloadObj(appAckPayloadObj);
  }
  return newMessage;
}

AppMessage *MessageFactory::makeAppMessageFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *commandTypeObj = cJSON_GetObjectItem(thisObj, "commandType");
  RETURN_IF_INVALID_CJSON_OBJ(commandTypeObj, NULL);
  AppMessageCommandType::Value commandType =
      static_cast<AppMessageCommandType::Value>(
          atoi(commandTypeObj->valuestring));

  cJSON *appPayloadObj = cJSON_GetObjectItem(thisObj, "payload");

  // Allocate and initialize a new AppMessage
  AppMessage *newMessage = new AppMessage(commandType);
  if (appPayloadObj != NULL) {
    newMessage->setAppPayloadObj(appPayloadObj);
  }
  return newMessage;
}

CompanionMessage *
MessageFactory::makeCompanionMessageFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *commandTypeObj = cJSON_GetObjectItem(thisObj, "commandType");
  RETURN_IF_INVALID_CJSON_OBJ(commandTypeObj, NULL);
  CompanionMessageCommandType::Value commandType =
      static_cast<CompanionMessageCommandType::Value>(
          atoi(commandTypeObj->valuestring));

  cJSON *companionPayloadObj = cJSON_GetObjectItem(thisObj, "payload");

  // Allocate and initialize a new CompanionMessage
  CompanionMessage *newMessage = new CompanionMessage(commandType);
  if (companionPayloadObj != NULL) {
    newMessage->setCompanionPayloadObj(companionPayloadObj);
  }
  return newMessage;
}

MLMessage *MessageFactory::makeMLMessageFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *commandTypeObj = cJSON_GetObjectItem(thisObj, "commandType");
  RETURN_IF_INVALID_CJSON_OBJ(commandTypeObj, NULL);
  MLMessageCommandType::Value commandType =
      static_cast<MLMessageCommandType::Value>(
          atoi(commandTypeObj->valuestring));

  cJSON *mlPayloadObj = cJSON_GetObjectItem(thisObj, "payload");

  // Allocate and initialize a new MLMessage
  MLMessage *newMessage = new MLMessage(commandType);
  if (mlPayloadObj != NULL) {
    newMessage->setMLPayloadObj(mlPayloadObj);
  }
  return newMessage;
}

MLAckMessage *
MessageFactory::makeMLAckMessageFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *commandMessageIdObj = cJSON_GetObjectItem(thisObj, "commandMessageId");
  RETURN_IF_INVALID_CJSON_OBJ(commandMessageIdObj, NULL);
  int commandMessageId = atoi(commandMessageIdObj->valuestring);

  cJSON *commandTypeObj = cJSON_GetObjectItem(thisObj, "commandType");
  RETURN_IF_INVALID_CJSON_OBJ(commandTypeObj, NULL);
  MLMessageCommandType::Value commandType =
      static_cast<MLMessageCommandType::Value>(
          atoi(commandTypeObj->valuestring));

  cJSON *mlAckPayloadObj = cJSON_GetObjectItem(thisObj, "payload");

  // Allocate and initialize a new MLAckMessage
  MLAckMessage *newMessage = new MLAckMessage(commandMessageId, commandType);
  if (mlAckPayloadObj != NULL) {
    newMessage->setMLAckPayloadObj(mlAckPayloadObj);
  }
  return newMessage;
}

ResourceRequest *
MessageFactory::makeResourceRequestFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *opTypeObj = cJSON_GetObjectItem(thisObj, "opType");
  RETURN_IF_INVALID_CJSON_OBJ(opTypeObj, NULL);
  ResourceOperationType::Value opType =
      ResourceOperationType::get(opTypeObj->valueint);

  cJSON *bodyObj = cJSON_GetObjectItem(thisObj, "body");
  RETURN_IF_INVALID_CJSON_OBJ(bodyObj, NULL);
  std::string body(bodyObj->valuestring);

  // Allocate and initialize a new ResourceRequest
  ResourceRequest *newMessage = new ResourceRequest(opType, body);
  return newMessage;
}

ResourceResponse *
MessageFactory::makeResourceResponseFromJSON(cJSON *messagePayloadObj) {
  cJSON *thisObj = messagePayloadObj;

  // Allocate and initialize a new message and return it
  cJSON *opTypeObj = cJSON_GetObjectItem(thisObj, "opType");
  RETURN_IF_INVALID_CJSON_OBJ(opTypeObj, NULL);
  ResourceOperationType::Value opType =
      ResourceOperationType::get(opTypeObj->valueint);

  cJSON *requestMessageIdObj = cJSON_GetObjectItem(thisObj, "reqMsgId");
  RETURN_IF_INVALID_CJSON_OBJ(requestMessageIdObj, NULL);
  int requestMessageId = requestMessageIdObj->valueint;

  cJSON *statusCodeObj = cJSON_GetObjectItem(thisObj, "statusCode");
  RETURN_IF_INVALID_CJSON_OBJ(statusCodeObj, NULL);
  int statusCode = statusCodeObj->valueint;

  cJSON *bodyObj = cJSON_GetObjectItem(thisObj, "body");
  RETURN_IF_INVALID_CJSON_OBJ(bodyObj, NULL);
  std::string body(bodyObj->valuestring);

  // Allocate and initialize a new ResourceRequest
  ResourceResponse *newMessage = new ResourceResponse(opType, requestMessageId, statusCode, body);
  return newMessage;
}