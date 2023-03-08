#
# Copyright (C) 2022 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk
include ../version.mk

PKG_NAME:=oui-rpc-core
PKG_VERSION:=$(strip $(call findrev))
PKG_RELEASE:=1

PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>
PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk

define Package/oui-rpc-core
  SECTION:=oui
  CATEGORY:=Oui
  TITLE:=Oui rpc core
  URL:=https://github.com/zhaojh329/oui
  DEPENDS:=+liblua +libuci-lua +lua-cjson +lua-eco-http +lua-eco-ubus +lua-md5 +px5g-mbedtls
endef

Build/Compile=

define Package/oui-rpc-core/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/usr/share/oui $(1)/usr/lib/lua/oui \
		$(1)/usr/share/oui/acl $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) ./files/oui-httpd.lua $(1)/usr/sbin/oui-httpd
	$(INSTALL_BIN) ./files/oui-httpd.init $(1)/etc/init.d/oui-httpd
	$(INSTALL_CONF) ./files/oui.config $(1)/etc/config/oui
	$(INSTALL_CONF) ./files/admin.acl $(1)/usr/share/oui/acl/admin.json
	$(CP) ./files/rpc.lua $(1)/usr/lib/lua/oui
	$(CP) ./files/rpc $(1)/usr/share/oui
endef

$(eval $(call BuildPackage,oui-rpc-core))
