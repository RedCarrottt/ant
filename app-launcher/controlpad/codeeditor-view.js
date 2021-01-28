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

var gMonacoEditor;
function CodeEditorView(
  onAppear,
  onRefresh,
  onSaveButton,
  onLaunchButton,
  onTerminateButton,
  onRemoveButton,
  updatePeriodMS = 1000
) {
  // Attributes
  this.mOnAppear = onAppear;
  this.mOnRefresh = onRefresh;
  this.mCommandBarHeight = 64;
  this.mRootDom = document.createElement('div');
  this.mUpdatePeriodMS = updatePeriodMS;

  // Command bar
  this.mCommandBar = new CommandBar(
    'command-bar',
    this.mCommandBarHeight,
    10,
    onSaveButton,
    onLaunchButton,
    onTerminateButton,
    onRemoveButton
  );
  this.append(this.mCommandBar);

  // Code Editor
  this.mCodeEditor = document.createElement('div');
  this.mCodeEditor.setAttribute('id', 'code_editor');
  this.append(this.mCodeEditor);
}

CodeEditorView.prototype.append = function (childView) {
  if (childView.getDom !== undefined) {
    this.mRootDom.append(childView.getDom());
  } else {
    this.mRootDom.append(childView);
  }
};

CodeEditorView.prototype.getDom = function () {
  return this.mRootDom;
};

CodeEditorView.prototype.onAddedDom = function () {
  // Initialize code editor on added the DOM element is added to root
  var self = this;
  var codeEditor = this.mCodeEditor;
  codeEditor.setAttribute(
    'style',
    'width: 100%; height: calc(100% - ' +
      this.mCommandBarHeight +
      'px); border: 1px solid grey'
  );
  if (gMonacoEditor === undefined) {
    require.config({paths: {vs: './vs'}});
    require(['vs/editor/editor.main'], function () {
      gMonacoEditor = monaco.editor.create(codeEditor, {
        value: '\n',
        language: 'javascript'
      });
      self.mOnAppear();
      self.startPeriodicUpdate();
    });
  } else {
    this.mOnAppear();
    this.startPeriodicUpdate();
  }
};

CodeEditorView.prototype.startPeriodicUpdate = function () {
  var self = this;
  this.mPeriodicUpdate = setInterval(function () {
    self.mOnRefresh();
  }, this.mUpdatePeriodMS);
};
CodeEditorView.prototype.endPeriodicUpdate = function () {
  if (this.mPeriodicUpdate !== undefined) clearInterval(this.mPeriodicUpdate);
};

CodeEditorView.prototype.setAppCode = function (appCode) {
  if (gMonacoEditor !== undefined) {
    gMonacoEditor.setValue(appCode);
  }
};

CodeEditorView.prototype.setCurrentAppState = function (state) {
  this.mCommandBar.setRunButtonState(state);
};

function CommandBar(
  id,
  height,
  margin,
  onSaveButton,
  onLaunchButton,
  onTerminateButton,
  onRemoveButton
) {
  var self = this;
  this.mId = id;
  this.mMargin = margin;
  this.mHeight = height - margin * 2;

  this.mOnSaveButton = onSaveButton;
  this.mOnLaunchButton = onLaunchButton;
  this.mOnTerminateButton = onTerminateButton;
  this.mOnRemoveButton = onRemoveButton;

  this.mRootDom = document.createElement('div');
  this.mRootDom.setAttribute('id', this.mId);
  this.mRootDom.setAttribute(
    'style',
    'height: ' + this.mHeight + 'px;' + ' margin: ' + this.mMargin + 'px;'
  );

  this.mSaveButton = new ButtonView('save-button', 'save', 'Save');
  this.mSaveButton.setOnClickHandler(function () {
    if (gMonacoEditor === undefined) {
      showErrorMessage('Code editor not loaded');
    }
    var appCode = gMonacoEditor.getValue();
    self.mOnSaveButton(appCode);
  });
  this.append(this.mSaveButton);

  this.mRunButton = new ButtonView('run-button', 'play_arrow', 'Run');
  this.mRunButton.setOnClickHandler(function () {
    var buttonText = self.mRunButton.getText();
    if (buttonText === 'Run') {
      self.mOnLaunchButton();
    } else {
      self.mOnTerminateButton();
    }
  });
  this.append(this.mRunButton);

  this.mRemoveButton = new ButtonView('remove-button', 'delete', 'Remove');
  this.mRemoveButton.setOnClickHandler(function () {
    self.mOnRemoveButton();
  });
  this.append(this.mRemoveButton);
}

CommandBar.prototype.append = function (childView) {
  if (childView.getDom !== undefined) {
    this.mRootDom.append(childView.getDom());
  } else {
    this.mRootDom.append(childView);
  }
};

CommandBar.prototype.getDom = function () {
  return this.mRootDom;
};

CommandBar.prototype.setRunButtonState = function (state) {
  if (state == 'Inactive') {
    this.mRunButton.setIconType('play_arrow');
    this.mRunButton.setText('Run');
    this.mRunButton.setDisabled(false);
  } else if (state == 'Active') {
    this.mRunButton.setIconType('pause');
    this.mRunButton.setText('Stop');
    this.mRunButton.setDisabled(false);
  } else if (state == 'Launching') {
    this.mRunButton.setIconType('hourglass_empty');
    this.mRunButton.setText('Launching');
    this.mRunButton.setDisabled(true);
  } else if (state == 'Terminating') {
    this.mRunButton.setIconType('hourglass_empty');
    this.mRunButton.setText('Terminating');
    this.mRunButton.setDisabled(true);
  } else {
    gUIController.showErrorMessage('Invalid state: ' + state);
  }
};
