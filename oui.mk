#
# Copyright (C) 2020 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

APP_NAME?=$(notdir ${CURDIR})
APP_SECTION?=oui
APP_CATEGORY?=Oui

PKG_NAME?=$(APP_NAME)
PKG_VERSION:=$(shell git --git-dir=$(CURDIR)/../../.git log -1 --pretty="%ci %h" | awk '{ print $$1 "-" $$4 }')
PKG_RELEASE?=1

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
	$(INSTALL_DIR) $(1)/www/views $(1)/usr/share/oui/menu.d
	$(INSTALL_DATA) ./files/menu.json $(1)/usr/share/oui/menu.d/$(APP_NAME).json
	$(INSTALL_DATA) ./vue/dist/app.js $(1)/www/views/$(APP_NAME).js
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
