#
# Copyright (C) 2019 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-app-upnp
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>

PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk

define Package/oui-app-upnp
  SECTION:=oui
  CATEGORY:=OUI
  SUBMENU:=Applications
  TITLE:=oui support for upnp
  DEPENDS:=+oui-ui-core +miniupnpd
  URL:=https://github.com/zhaojh329/oui
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Package/oui-app-upnp/install
	$(INSTALL_DIR) $(1)/www/views/services $(1)/www/i18n/services \
		$(1)/usr/share/rpcd/acl.d $(1)/usr/share/oui/menu.d \
		$(1)/usr/libexec/rpcd
	$(CP) ./files/upnp.menu $(1)/usr/share/oui/menu.d/upnp.json
	$(CP) ./files/upnp.acl $(1)/usr/share/rpcd/acl.d/upnp.json
	$(CP) ./files/upnp.i18n $(1)/www/i18n/services/upnp.json
	$(INSTALL_BIN) ./files/upnp.rpc $(1)/usr/libexec/rpcd/upnp
	$(CP) $(PKG_BUILD_DIR)/dist/plugin.common.js $(1)/www/views/services/upnp.js
endef

$(eval $(call BuildPackage,oui-app-upnp))
