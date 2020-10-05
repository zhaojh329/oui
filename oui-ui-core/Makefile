#
# Copyright (C) 2017 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk
include ../oui.mk

PKG_NAME:=oui-ui-core
PKG_VERSION:=$(strip $(call findrev))
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>

PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk

define Package/oui-ui-core
  SECTION:=oui
  CATEGORY:=Oui
  TITLE:=Oui ui core
  DEPENDS:=+oui-httpd +oui-bwm
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-ui-core/description
 Provides core ui for oui
endef

Build/Prepare=

Build/Compile=

define Package/oui-ui-core/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/usr/share/oui/menu.d $(1)/www
	$(INSTALL_CONF) ./files/oui.menu $(1)/usr/share/oui/menu.d/oui.json
	$(INSTALL_CONF) ./files/oui.config $(1)/etc/config/oui
	$(CP) ./files/i18n $(1)/www
	$(CP) ./vue/dist/* $(1)/www
endef

$(eval $(call BuildPackage,oui-ui-core))
