define ColorInfo
	tput setaf 6;echo $1;tput sgr0
endef

define ColorError
	tput setaf 1;echo $1;tput sgr0
endef

ifneq ($(CONFIG_OUI_USE_HOST_NODE),)
OUI_NODE_PATH := $(subst ",,$(CONFIG_OUI_HOST_NODE_PATH))
ifneq ($(OUI_NODE_PATH),)
OUI_NODE_PATH := PATH=$(OUI_NODE_PATH)
else
OUI_NODE_PATH := PATH=$(shell echo $(PATH) | sed -E 's|$(TOPDIR)/staging_dir/host/bin:||g')
endif
NODE := $(OUI_NODE_PATH) node
NPM := $(OUI_NODE_PATH) npm
else
NODE := node
NPM := npm
endif

NODE_VER_MIN := 14.18

NODE_VER := $(shell $(NODE) -v | sed 's/v//' | awk -F. '{print $$1"."$$2}')

define OuiCheckNode
	@$(call ColorInfo, "Checking Node.js for build oui...")
	@if [ -n "$(CONFIG_OUI_USE_HOST_NODE)" ]; \
	then \
		$(call ColorInfo, "Using Node.js from Host"); \
	else \
		$(call ColorInfo, "Using Node.js from OpenWrt"); \
	fi
	@if [ -z "$(NODE_VER)" ]; \
	then \
		$(call ColorError, "Node.js $(NODE_VER_MIN)+ is required"); \
		false; \
	fi
	@$(call ColorInfo, "Node.js version: $(NODE_VER)")
	@if [ "$(shell echo $(NODE_VER) | awk '{if ($$1 >= $(NODE_VER_MIN)) { printf "ok" }}')" != "ok" ]; \
	then \
		$(call ColorError, "Node.js $(NODE_VER_MIN)+ is required"); \
		false; \
    fi
endef
