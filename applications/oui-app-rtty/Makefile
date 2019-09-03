#
# Copyright (C) 2019 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-app-rtty
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>

PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk

define Package/oui-app-rtty
  SECTION:=oui
  CATEGORY:=OUI
  SUBMENU:=Applications
  TITLE:=oui support for rtty
  DEPENDS:=+oui-ui-core
  URL:=https://github.com/zhaojh329/rtty
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Package/oui-app-rtty/install
	$(INSTALL_DIR) $(1)/www/views/services $(1)/www/i18n/services \
		$(1)/usr/share/rpcd/acl.d $(1)/usr/share/oui/menu.d
	$(CP) ./files/rtty.menu $(1)/usr/share/oui/menu.d/rtty.json
	$(CP) ./files/rtty.acl $(1)/usr/share/rpcd/acl.d/rtty.json
	$(CP) ./files/rtty.i18n $(1)/www/i18n/services/rtty.json
	$(CP) $(PKG_BUILD_DIR)/dist/plugin.common.js $(1)/www/views/services/rtty.js
endef

$(eval $(call BuildPackage,oui-app-rtty))
