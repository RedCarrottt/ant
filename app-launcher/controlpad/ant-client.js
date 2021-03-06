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

function ANTClient() {}

ANTClient.prototype._sendHTTPRequest = function (
  url,
  method,
  data,
  responseHandler
) {
  var xhr = new XMLHttpRequest();
  xhr.open(method, url, true);
  xhr.setRequestHeader('Content-Type', 'application/json');
  xhr.onload = function () {
    if (responseHandler !== undefined) {
      responseHandler(this.status, this.responseText);
    }
  };
  if (data === undefined) {
    data = null;
  }
  xhr.send(data);
};

ANTClient.prototype.getAppList = function (handler) {
  var url = 'http://' + this.getTargetUri() + '/apps';
  this._sendHTTPRequest(
    url,
    'GET',
    undefined,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        var appList = JSON.parse(responseText);
        handler(true, appList);
      } else {
        handler(false);
      }
    }
  );
};

ANTClient.prototype.installApp = function (appName, appCode, handler) {
  var url = 'http://' + this.getTargetUri() + '/apps';
  var data = appName + '\n' + appCode;
  this._sendHTTPRequest(
    url,
    'POST',
    data,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        handler(true, responseText);
      } else {
        handler(false, responseText);
      }
    }
  );
};

ANTClient.prototype.getAppCode = function (appName, handler) {
  var url = 'http://' + this.getTargetUri() + '/apps/' + appName + '/code';
  this._sendHTTPRequest(
    url,
    'GET',
    undefined,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        var appCode = responseText;
        handler(true, appCode);
      } else {
        handler(false);
      }
    }
  );
};

ANTClient.prototype.getAppState = function (appName, handler) {
  var url = 'http://' + this.getTargetUri() + '/apps/' + appName + '/state';
  this._sendHTTPRequest(
    url,
    'GET',
    undefined,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        var appState = responseText;
        handler(true, appState);
      } else {
        handler(false);
      }
    }
  );
};

ANTClient.prototype.removeApp = function (appName, handler) {
  var url = 'http://' + this.getTargetUri() + '/apps/' + appName;
  this._sendHTTPRequest(
    url,
    'DELETE',
    undefined,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        handler(true, responseText);
      } else {
        handler(false, responseText);
      }
    }
  );
};

ANTClient.prototype.launchApp = function (appName, handler) {
  var url = 'http://' + this.getTargetUri() + '/apps/' + appName + '/launch';
  this._sendHTTPRequest(
    url,
    'POST',
    undefined,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        handler(true, responseText);
      } else {
        handler(false, responseText);
      }
    }
  );
};

ANTClient.prototype.terminateApp = function (appName, handler) {
  var url = 'http://' + this.getTargetUri() + '/apps/' + appName + '/terminate';
  this._sendHTTPRequest(
    url,
    'POST',
    undefined,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        handler(true, responseText);
      } else {
        handler(false, responseText);
      }
    }
  );
};

ANTClient.prototype.getOutput = function (appName, fromTs, handler) {
  var url = 'http://' + this.getTargetUri() + '/apps/' + appName + '/output';
  var data = '' + fromTs;
  this._sendHTTPRequest(
    url,
    'POST',
    data,
    function (responseCode, responseText) {
      if (responseCode == 200) {
        var outputs = JSON.parse(responseText);
        handler(true, outputs);
      } else {
        handler(false);
      }
    }
  );
};
