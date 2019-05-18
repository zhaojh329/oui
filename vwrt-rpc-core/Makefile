#
# Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vwrt-rpc-core
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <jianhuizhao329@gmail.com>

PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/vwrt-rpc-core
  SECTION:=vwrt
  CATEGORY:=VWRT
  TITLE:=Base VWRT RPC calls
  DEPENDS:=+rpcd +rpcd-mod-iwinfo +rpcd-mod-rpcsys +uhttpd +uhttpd-mod-ubus
  URL:=https://github.com/zhaojh329/vwrt
endef

define Package/vwrt-rpc-core/description
 Provides the core RPC infrastructure to run VWRT.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/vwrt-rpc-core/install
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vwrt.so $(1)/usr/lib/rpcd/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bwmon-vwrt.so $(1)/usr/lib/rpcd/
endef

define Package/vwrt-rpc-core/postinst
#!/bin/sh
killall -HUP rpcd 2>/dev/null
exit 0
endef

$(eval $(call BuildPackage,vwrt-rpc-core))
