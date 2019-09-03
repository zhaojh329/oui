#
# Copyright (C) 2017 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-rpc-core
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>

PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/oui-rpc-core
  SECTION:=oui
  CATEGORY:=OUI
  TITLE:=Base OUI RPC calls
  DEPENDS:=+rpcd +rpcd-mod-iwinfo +rpcd-mod-file +rpcd-mod-rpcsys +uhttpd \
		+uhttpd-mod-ubus +lua +libubox-lua +libubus-lua \
		+libuci-lua +lua-cjson +liblua +kmod-oui-bwm
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-rpc-core/description
 Provides the core RPC infrastructure to run OUI.
endef

define Package/oui-rpc-core/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/usr/lib/lua $(1)/etc/config
	$(INSTALL_CONF) ./files/oui-bwm.config $(1)/etc/config/oui-bwm
	$(INSTALL_BIN) ./files/oui.lua $(1)/usr/sbin/oui
	$(INSTALL_BIN) ./files/oui.init $(1)/etc/init.d/oui
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/oui.so $(1)/usr/lib/lua
endef

$(eval $(call BuildPackage,oui-rpc-core))
