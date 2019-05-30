#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-io-util
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0+

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/oui-io-util
  SECTION:=oui
  CATEGORY:=OUI
  DEPENDS:=+libubox +libubus
  TITLE:=CGI utility for handling up/downloading of files
endef

define Package/oui-io-util/description
  This package contains an cgi utility that is useful for up/downloading files
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/oui-io-util/install
	$(INSTALL_DIR) $(1)/usr/libexec $(1)/www/cgi-bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/oui-io $(1)/usr/libexec
	$(LN) /usr/libexec/oui-io $(1)/www/cgi-bin/oui-upload 
	$(LN) /usr/libexec/oui-io $(1)/www/cgi-bin/oui-backup
endef

$(eval $(call BuildPackage,oui-io-util))
