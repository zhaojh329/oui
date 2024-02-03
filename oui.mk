#
# Copyright (C) 2022 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include ${CURDIR}/../../version.mk
include ${CURDIR}/../../node.mk

PKG_NAME:=$(notdir ${CURDIR})
PKG_VERSION:=$(strip $(call findrev))
PKG_RELEASE?=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=oui
  CATEGORY:=Oui
  SUBMENU:=Applications
  TITLE:=$(APP_TITLE)
  DEPENDS:=+oui-ui-core $(APP_DEPENDS)
  PKGARCH:=all
endef

define Build/Prepare
	if [ -d ./htdoc ]; then \
		$(CP) ./htdoc $(PKG_BUILD_DIR); \
		echo "VITE_APP_NAME=$(APP_NAME)" > $(PKG_BUILD_DIR)/htdoc/.env.local; \
	fi
endef

define Build/Compile
	if [ -d $(PKG_BUILD_DIR)/htdoc ]; then \
		$(NPM) --prefix $(PKG_BUILD_DIR)/htdoc install && \
		$(NPM) --prefix $(PKG_BUILD_DIR)/htdoc run build && \
		$(RM) -rf $(PKG_BUILD_DIR)/htdoc/node_modules; \
	fi
endef

define Package/$(PKG_NAME)/install
	if [ -d $(PKG_BUILD_DIR)/htdoc/dist ]; then \
		$(INSTALL_DIR) $(1)/www/views; \
		$(CP) $(PKG_BUILD_DIR)/htdoc/dist/* $(1)/www/views; \
	fi
	if [ -f ./files/menu.json ]; then \
		$(INSTALL_DIR) $(1)/usr/share/oui/menu.d; \
		$(INSTALL_CONF) ./files/menu.json $(1)/usr/share/oui/menu.d/$(APP_NAME).json; \
	fi
	if [ -d ./files/rpc ]; then \
		$(CP) ./files/rpc $(1)/usr/share/oui/rpc; \
	fi
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
