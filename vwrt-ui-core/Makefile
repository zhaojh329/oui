#
# Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vwrt-ui-core
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <jianhuizhao329@gmail.com>

PKG_LICENSE:=MIT

PKG_BUILD_PARALLEL:=1

PKG_BUILD_DEPENDS:=!VWRT_USE_HOST_NODE:node/host
PKG_CONFIG_DEPENDS:=CONFIG_VWRT_USE_HOST_NODE CONFIG_VWRT_BUILD_DEV

include $(INCLUDE_DIR)/package.mk

define Package/vwrt-ui-core
  SECTION:=vwrt
  CATEGORY:=VWRT
  TITLE:=vwrt ui core
  DEPENDS:=+vwrt-rpc-core
  URL:=https://github.com/zhaojh329/vwrt
endef

define Package/vwrt-ui-core/description
 Provides core ui for VWRT
endef

define Package/vwrt-ui-core/config
menu "Configuration"
	depends on PACKAGE_vwrt-ui-core

	config VWRT_USE_HOST_NODE
		bool "Use existing nodejs installation on the host system"
		default n
		help
		  This disables the build dependency on the node package from
		  the OpenWrt packages feed, allowing for faster development
		  builds.
	config VWRT_BUILD_DEV
		bool "Build for development"
		default n
		help
		  This turn on development mode. During development, Vue provides
		  a lot of warnings to help you with common errors and pitfalls.
endmenu
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

ifneq ($(CONFIG_VWRT_BUILD_DEV),)
  MAKE_VARS=NODE_ENV=development
endif

define Package/vwrt-ui-core/install
	$(INSTALL_DIR) $(1)/www
	$(CP) $(PKG_BUILD_DIR)/dist/* $(1)/www/
	$(CP) ./files/* $(1)/
endef

define Package/vwrt-ui-core/postinst
#!/bin/sh
uci batch <<-EOF
	set uhttpd.main.ubus_prefix="/ubus"
	set uhttpd.main.index_page='vwrt.html'
	commit uhttpd
EOF
/etc/init.d/uhttpd restart &
exit 0
endef

$(eval $(call BuildPackage,vwrt-ui-core))
