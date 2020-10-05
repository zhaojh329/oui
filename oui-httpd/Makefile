#
# Copyright (C) 2020 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk
include ../oui.mk

PKG_NAME:=oui-httpd
PKG_VERSION:=$(strip $(call findrev))
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>

PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/oui-httpd
  SECTION:=oui
  CATEGORY:=Oui
  TITLE:=Oui rpc backend
  DEPENDS:=+libuhttpd-nossl +lua +libubox-lua +libubus-lua +libuci-lua +jansson
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-httpd/description
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)
endef

define Package/oui-httpd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config \
		$(1)/usr/lib/oui-httpd/rpc $(1)/usr/lib/oui-httpd/lib
	$(INSTALL_CONF) ./files/oui-httpd.config $(1)/etc/config/oui-httpd
	$(INSTALL_BIN) ./files/oui-httpd.init $(1)/etc/init.d/oui-httpd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/oui-httpd $(1)/usr/sbin
	$(INSTALL_DATA) ./files/utils.lua $(1)/usr/lib/oui-httpd/lib
	for f in `ls ./files/rpc`; do \
		$(INSTALL_DATA) ./files/rpc/$$$$f $(1)/usr/lib/oui-httpd/rpc/$$$${f%.*}; \
	done
endef

$(eval $(call BuildPackage,oui-httpd))
