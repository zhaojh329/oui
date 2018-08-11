#
# Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
# Copyright (C) 2015-2016 Jo-Philipp Wich <jow@openwrt.org>
#
# This is free software, licensed under the GNU General Public License v3.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vuci-rpc-base
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <jianhuizhao329@gmail.com>

PKG_LICENSE:=LGPL-2.1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/vuci-rpc-base
  SECTION:=vuci
  CATEGORY:=VUCI
  TITLE:=Base VUCI RPC calls
  DEPENDS:=+rpcd +rpcd-mod-iwinfo +rpcd-mod-rpcsys +uhttpd +uhttpd-mod-ubus
endef

define Package/vuci-rpc-base/description
 Provides the basic RPC infrastructure to run VUCI.
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/vuci-rpc-base/install
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vuci.so $(1)/usr/lib/rpcd/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bwmon-vuci.so $(1)/usr/lib/rpcd/
endef

define Package/vuci-rpc-base/postinst
#!/bin/sh
killall -HUP rpcd 2>/dev/null
exit 0
endef

$(eval $(call BuildPackage,vuci-rpc-base))
