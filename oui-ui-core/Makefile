#
# Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-ui-core
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <jianhuizhao329@gmail.com>

PKG_LICENSE:=MIT

PKG_BUILD_PARALLEL:=1

ifeq ($(CONFIG_OUI_USE_HOST_NODE),)
ifneq ($(CONFIG_OUI_USE_BUILT_IN_NODE),)
PKG_BUILD_DEPENDS:=node/host
else
PKG_BUILD_DEPENDS:=oui-node/host
endif
endif

PKG_CONFIG_DEPENDS:=CONFIG_OUI_USE_HOST_NODE CONFIG_OUI_BUILD_DEV

include $(INCLUDE_DIR)/package.mk

define Package/oui-ui-core
  SECTION:=oui
  CATEGORY:=OUI
  TITLE:=oui ui core
  DEPENDS:=+oui-rpc-core
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-ui-core/description
 Provides core ui for OUI
endef

define Package/oui-ui-core/config
menu "Configuration"
	depends on PACKAGE_oui-ui-core

	config OUI_USE_HOST_NODE
		bool "Use existing nodejs installation on the host system"
		default n
		help
		  This disables the build dependency on the node package from
		  the OpenWrt packages feed, allowing for faster development
		  builds.
	config OUI_BUILD_DEV
		bool "Build for development"
		default n
		help
		  This turn on development mode. During development, Vue provides
		  a lot of warnings to help you with common errors and pitfalls.

	config OUI_USE_BUILT_IN_NODE
		bool "Use oUI built-in node version"
		default y
		depends on !OUI_USE_HOST_NODE
		help
			Select this option will be use oUI built in node version for
			compile VUE code
endmenu
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

ifneq ($(CONFIG_OUI_BUILD_DEV),)
  MAKE_VARS=NODE_ENV=development
endif

define Package/oui-ui-core/install
	$(INSTALL_DIR) $(1)/www
	$(CP) $(PKG_BUILD_DIR)/dist/* $(1)/www/
	$(CP) ./files/* $(1)/
endef

define Package/oui-ui-core/postinst
#!/bin/sh
uci batch <<-EOF
	set uhttpd.main.ubus_prefix="/ubus"
	set uhttpd.main.index_page='oui.html'
	commit uhttpd
EOF
/etc/init.d/uhttpd restart &
exit 0
endef

$(eval $(call BuildPackage,oui-ui-core))
