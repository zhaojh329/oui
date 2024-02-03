#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lua-cjson-53
PKG_VERSION:=2.1.0
PKG_RELEASE:=1
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/lua-cjson-53
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Lua5.3 CJSON parser
  URL:=https://github.com/mpx/lua-cjson
  DEPENDS:= +liblua5.3
endef

define Package/lua-cjson-53/description
  Lua5.3 CJSON is a fast JSON encoding/parsing module for Lua.
endef

TARGET_CFLAGS += $(FPIC) -I$(STAGING_DIR)/usr/include/lua5.3
TARGET_LDFLAGS += -lm

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -shared \
		$(PKG_BUILD_DIR)/{fpconv.c,strbuf.c,lua_cjson.c} \
		-o $(PKG_BUILD_DIR)/cjson.so
endef

define Package/lua-cjson-53/install
	$(INSTALL_DIR) $(1)/usr/local/lib/lua/5.3
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cjson.so $(1)/usr/local/lib/lua/5.3
endef

$(eval $(call BuildPackage,lua-cjson-53))
