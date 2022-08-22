#
# Copyright (C) 2022 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk
include ../version.mk

PKG_NAME:=oui-ui-core
PKG_VERSION:=$(strip $(call findrev))
PKG_RELEASE:=1

PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>
PKG_LICENSE:=MIT

PKG_BUILD_DEPENDS:=!OUI_USE_HOST_NODE:node/host
PKG_CONFIG_DEPENDS:=CONFIG_OUI_USE_HOST_NODE

ifeq ($(CONFIG_OUI_USE_HOST_NODE),)
NPM:=$(STAGING_DIR_HOSTPKG)/bin/npm
endif

NPM?=npm

include $(INCLUDE_DIR)/package.mk

define Package/oui-ui-core
  SECTION:=oui
  CATEGORY:=Oui
  TITLE:=Oui ui core
  URL:=https://github.com/zhaojh329/oui
  DEPENDS:=+oui-rpc-core
  PKGARCH:=all
endef

define Package/oui-ui-core/config
  config OUI_USE_HOST_NODE
	bool "Use existing nodejs installation on the host system"
	default n
	help
	  This disables the build dependency on the node package from
	  the OpenWrt packages feed, allowing for faster development
	  builds.
endef

define Package/oui-ui-core/conffiles
/etc/config/oui
endef

define Build/Prepare
	$(CP) ./htdoc $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(NPM) --prefix $(PKG_BUILD_DIR)/htdoc install
	$(NPM) --prefix $(PKG_BUILD_DIR)/htdoc run build
endef

define Package/oui-ui-core/install
	$(INSTALL_DIR) $(1)/usr/share/oui/menu.d $(1)/www
	$(INSTALL_CONF) ./files/menu.json $(1)/usr/share/oui/menu.d/base.json
	$(CP) $(PKG_BUILD_DIR)//htdoc/dist/* $(1)/www
endef

$(eval $(call BuildPackage,oui-ui-core))
