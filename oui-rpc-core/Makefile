#
# Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-rpc-core
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <jianhuizhao329@gmail.com>

PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/oui-rpc-core
  SECTION:=oui
  CATEGORY:=OUI
  TITLE:=Base OUI RPC calls
  DEPENDS:=+rpcd +rpcd-mod-iwinfo +rpcd-mod-rpcsys +uhttpd +uhttpd-mod-ubus
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-rpc-core/description
 Provides the core RPC infrastructure to run OUI.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/oui-rpc-core/install
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/oui.so $(1)/usr/lib/rpcd/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bwmon-oui.so $(1)/usr/lib/rpcd/
endef

define Package/oui-rpc-core/postinst
#!/bin/sh
killall -HUP rpcd 2>/dev/null
exit 0
endef

$(eval $(call BuildPackage,oui-rpc-core))
