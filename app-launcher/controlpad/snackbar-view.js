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

function SnackbarView() {
  View.apply(this, ['snack-bar']);

  this.SHORT_MS = 2000;
  this.LONG_MS = 4000;
}
SnackbarView.prototype = Object.create(View.prototype);
SnackbarView.prototype.constructor = SnackbarView;

SnackbarView.prototype.showShort = function (
  message,
  handler,
  actionText,
  type
) {
  this.show(message, handler, this.SHORT_MS, actionText, type);
};

SnackbarView.prototype.showLong = function (
  message,
  handler,
  actionText,
  type
) {
  this.show(message, handler, this.LONG_MS, actionText, type);
};

SnackbarView.prototype.show = function (
  message,
  handler,
  timeoutMS,
  actionText = 'Close',
  type = 'INFO'
) {
  var data = {
    message: message,
    timeout: timeoutMS,
    actionHandler: handler,
    actionText: actionText
  };

  if (this.mRootDom.MaterialSnackbar !== undefined) {
    if (this.mRootDom.classList.contains('snackbar-error')) {
      this.mRootDom.classList.remove('snackbar-error');
    }
    if (this.mRootDom.classList.contains('snackbar-success')) {
      this.mRootDom.classList.remove('snackbar-success');
    }

    if (type == 'ERROR') {
      this.mRootDom.classList.add('snackbar-error');
    } else if (type == 'SUCCESS') {
      this.mRootDom.classList.add('snackbar-success');
    }

    this.mRootDom.MaterialSnackbar.showSnackbar(data);
  }
};
