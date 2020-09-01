#
# Copyright (C) 2017 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oui-ui
PKG_VERSION:=$(shell git --git-dir=$(CURDIR)/../.git log -1 --pretty="%ci %h" | awk '{ print $$1 "-" $$4 }')
PKG_RELEASE:=2
PKG_MAINTAINER:=Jianhui Zhao <zhaojh329@gmail.com>

PKG_LICENSE:=MIT

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/oui-ui
  SECTION:=oui
  CATEGORY:=OUI
  TITLE:=oui web interface
  DEPENDS:=+oui-httpd
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-ui/description
 Provides core ui for OUI
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/oui-ui/install
	$(INSTALL_DIR) $(1)/www
	$(CP) ./files/* $(1)/
	$(CP) ./dist/* $(1)/www/
	for f in `find $(1)/www -name *.gz`; do \
		mv $$$$f $$$${f%.*}; \
	done
endef

$(eval $(call BuildPackage,oui-ui))
