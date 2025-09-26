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
  DEPENDS:=+lua-cjson-lua5.4 +lua-eco-uci +lua-eco-md5 +lua-eco-ubus +lua-eco-log \
  		+lua-eco-socket +lighttpd +lighttpd-mod-scgi +lighttpd-mod-magnet
endef

Build/Compile=

define Package/oui-rpc-core/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) ./files/oui.lua $(1)/usr/sbin/oui
	$(INSTALL_BIN) ./files/oui.init $(1)/etc/init.d/oui
	$(INSTALL_CONF) ./files/oui.config $(1)/etc/config/oui

	$(INSTALL_DIR) $(1)/usr/local/lib/lua/5.4/oui
	$(CP) ./files/rpc.lua $(1)/usr/local/lib/lua/5.4/oui

	$(INSTALL_DIR) $(1)/usr/share/oui/acl
	$(CP) ./files/rpc $(1)/usr/share/oui
	$(INSTALL_CONF) ./files/admin.acl $(1)/usr/share/oui/acl/admin.json

	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/oui.default $(1)/etc/uci-defaults/50-oui

	$(INSTALL_DIR) $(1)/etc/lighttpd/conf.d
	$(INSTALL_DATA) ./files/lighttpd/oui.conf $(1)/etc/lighttpd/conf.d/50-oui.conf
	$(INSTALL_DATA) ./files/lighttpd/handler.lua $(1)/etc/lighttpd/oui-handler.lua
endef

$(eval $(call BuildPackage,oui-rpc-core))
