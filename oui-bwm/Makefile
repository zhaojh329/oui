#
# Copyright (C) 2019 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=oui-bwm
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define KernelPackage/oui-bwm
  SUBMENU:=Network Support
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

define KernelPackage/oui-bwm/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_CONF) ./files/oui-bwm.config $(1)/etc/config/oui-bwm
	$(INSTALL_BIN) ./files/oui-bwm.init $(1)/etc/init.d/oui-bwm
endef

$(eval $(call KernelPackage,oui-bwm))
