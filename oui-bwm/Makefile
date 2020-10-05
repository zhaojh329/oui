#
# Copyright (C) 2019 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include ../oui.mk

PKG_NAME:=oui-bwm
PKG_VERSION:=$(strip $(call findrev))
PKG_RELEASE:=1
PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk

define Package/oui-bwm
  SECTION:=oui
  CATEGORY:=Oui
  TITLE:=Bandwidth Monitor for oui
  DEPENDS:=+kmod-oui-bwm
  URL:=https://github.com/zhaojh329/oui
endef

define Package/oui-bwm/description
 Bandwidth Monitor for oui
endef

define KernelPackage/oui-bwm
  SUBMENU:=Other modules
  TITLE:=Bandwidth Monitor for oui
  FILES:=$(PKG_BUILD_DIR)/oui-bwm.ko
  AUTOLOAD:=$(call AutoLoad,30,oui-bwm,1)
  KCONFIG:=
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)
endef

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

define Package/oui-bwm/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_CONF) ./files/oui-bwm.config $(1)/etc/config/oui-bwm
	$(INSTALL_BIN) ./files/oui-bwm.init $(1)/etc/init.d/oui-bwm
endef

$(eval $(call BuildPackage,oui-bwm))
$(eval $(call KernelPackage,oui-bwm))
