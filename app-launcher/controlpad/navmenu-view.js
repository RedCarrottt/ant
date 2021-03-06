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

/* NavMenuView */
function NavMenuView(context) {
  View.apply(this, ['nav-menu']);

  // Set attributes
  this.mNavItems = [];
  this.mContext = context;
}
NavMenuView.prototype = Object.create(View.prototype);
NavMenuView.prototype.constructor = NavMenuView;

NavMenuView.prototype.getItem = function (key) {
  if (typeof key === 'number') {
    return this.mNavItems[key];
  } else if (typeof key === 'string') {
    for (var i = 0; i < this.mNavItems.length; i++) {
      var item = this.mNavItems[i];
      if (item.getId() === key) {
        return item;
      }
    }
  } else {
    return undefined;
  }
};

NavMenuView.prototype.selectItem = function (navItemView) {
  // Update context
  var prevNavItemView = this.mContext.currentNavItem;
  this.mContext.currentNavItem = navItemView;

  // Update header title
  var titleText = 'ANT Controlpad: ' + navItemView.getTitle();
  $('#header_title').html(titleText);

  // Update content view
  $('#content_root').html(navItemView.getView().getDom());

  // Call handlers
  if (
    prevNavItemView !== undefined &&
    prevNavItemView.getView().onRemovedDom !== undefined
  ) {
    prevNavItemView.getView().onRemovedDom();
  }
  if (navItemView.getView().onAddedDom !== undefined) {
    navItemView.getView().onAddedDom();
  }
};

NavMenuView.prototype.addItem = function (id, iconType, title, view) {
  var self = this;
  var onClickNavItem = function (e) {
    var navItem = undefined;
    for (var j in self.mNavItems) {
      if (self.mNavItems[j].getId() == e.target.id) {
        navItem = self.mNavItems[j];
        break;
      }
    }
    if (navItem !== undefined) {
      self.selectItem(navItem);
    }
  };

  // New nav menu item
  var navItemView = new NavMenuItemView(id, iconType, title, view);
  navItemView.getDom().onclick = onClickNavItem;

  // Add navItemView to the list
  this.mNavItems.push(navItemView);

  // Add to DOM tree
  this.append(navItemView);
};

/* NavMenuItemView */
function NavMenuItemView(id, iconType, title, view) {
  View.apply(this, [id, 'a']);

  this.mIconType = iconType;
  this.mTitle = title;
  this.mView = view;

  this.mRootDom.setAttribute('class', 'mdl-navigation__link');
  this.mRootDom.setAttribute('href', '#');
  {
    var icon = document.createElement('i');
    icon.setAttribute('class', 'mdl-color-text--blue-grey-400 material-icons');
    icon.setAttribute('role', 'presentation');
    icon.innerHTML = this.mIconType;
    this.append(icon);
    this.append(this.mTitle);
  }
}
NavMenuItemView.prototype = Object.create(View.prototype);
NavMenuItemView.prototype.constructor = NavMenuItemView;

NavMenuItemView.prototype.getId = function () {
  return this.mId;
};

NavMenuItemView.prototype.getTitle = function () {
  return this.mTitle;
};

NavMenuItemView.prototype.getView = function () {
  return this.mView;
};
