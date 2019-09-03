#
# Copyright (C) 2017 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-ui-core
PKG_VERSION:=$(shell git --git-dir=$(CURDIR)/../.git log -1 --pretty="%ci %h" | awk '{ print $$1 "-" $$4 }')
PKG_RELEASE:=1
PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>

PKG_LICENSE:=MIT

PKG_BUILD_PARALLEL:=1

PKG_BUILD_DEPENDS:=OUI_USE_OUI_NODE:oui-node/host OUI_USE_OW_NODE:node/host

PKG_CONFIG_DEPENDS:=CONFIG_OUI_USE_HOST_NODE CONFIG_OUI_BUILD_DEV CONFIG_OUI_USE_OUI_NODE

include $(INCLUDE_DIR)/package.mk

define Package/oui-ui-core
  SECTION:=oui
  CATEGORY:=OUI
  TITLE:=oui ui core
  DEPENDS:=+oui-rpc-core +oui-io-util
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-ui-core/description
 Provides core ui for OUI
endef

define Package/oui-ui-core/config
	source $(SOURCE)/Config.in
endef


define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
	ln -sf $(PKG_BUILD_DIR) $(BUILD_DIR)/.oui-build-plugin
endef

ifneq ($(CONFIG_OUI_BUILD_DEV),)
  MAKE_VARS = NODE_ENV=development
endif

ifneq ($(CONFIG_OUI_USE_HOST_NODE),)
  MAKE_VARS += NODE=$(shell PATH=/usr/bin:/usr/local/bin which node)
  MAKE_VARS += NPM=$(shell PATH=/usr/bin:/usr/local/bin which npm)
else
  MAKE_VARS += NODE=$(STAGING_DIR_HOST)/bin/node
  MAKE_VARS += NPM=$(STAGING_DIR_HOST)/bin/npm
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
