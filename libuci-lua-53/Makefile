#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libuci-lua53
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/libuci-lua53
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Lua5.3 plugin for UCI
  DEPENDS:=+libuci +liblua5.3
endef

TARGET_CFLAGS += $(FPIC) -I$(STAGING_DIR)/usr/include/lua5.3
TARGET_LDFLAGS += -luci

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) -shared \
		$(PKG_BUILD_DIR)/uci.c \
		-o $(PKG_BUILD_DIR)/uci.so
endef

define Package/libuci-lua53/install
	$(INSTALL_DIR) $(1)/usr/local/lib/lua/5.3
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uci.so $(1)/usr/local/lib/lua/5.3
endef

$(eval $(call BuildPackage,libuci-lua53))
