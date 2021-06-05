#
# Copyright (C) 2020 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

TOPDIR:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include $(TOPDIR)/oui.mk

APP_NAME?=$(notdir ${CURDIR})
APP_SECTION?=oui
APP_CATEGORY?=Oui
APP_VIEW?=$(APP_NAME)

PKG_NAME?=$(APP_NAME)
PKG_RELEASE?=1

PKG_SRC_VERSION?=$(strip $(call findrev))
PKG_VERSION:=$(if $(PKG_VERSION),$(PKG_VERSION),$(PKG_SRC_VERSION))

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=$(APP_SECTION)
  CATEGORY:=$(APP_CATEGORY)
  SUBMENU:=Applications
  TITLE:=$(if $(APP_TITLE),$(APP_TITLE),$(APP_NAME))
  DEPENDS:=+oui-ui-core $(APP_DEPENDS)
endef

Build/Compile=

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/www/views
	$(INSTALL_DATA) ./vue/dist/app.js $(1)/www/views/$(APP_VIEW).js
	if [ -f ./files/menu.json ];then \
	  $(INSTALL_DIR) $(1)/usr/share/oui/menu.d; \
	  $(INSTALL_DATA) ./files/menu.json $(1)/usr/share/oui/menu.d/$(APP_NAME).json; \
	fi
	if [ -d ./files/i18n ];then \
	  $(CP) ./files/i18n $(1)/www/i18n; \
	fi
	if [ -d ./files/rpc ];then \
	  $(INSTALL_DIR) $(1)/usr/lib/oui-httpd/rpc; \
	  for f in `ls ./files/rpc`; do \
	    $(INSTALL_DATA) ./files/rpc/$$$$f $(1)/usr/lib/oui-httpd/rpc/$$$${f%.*}; \
	  done \
	fi
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
