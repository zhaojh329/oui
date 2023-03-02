define ColorInfo
	tput setaf 6;echo $1;tput sgr0
endef

define ColorError
	tput setaf 1;echo $1;tput sgr0
endef

ifneq ($(CONFIG_OUI_USE_HOST_NODE),)
NODE_PATH := PATH=$(PATH)
else
NODE_PATH := PATH=$(STAGING_DIR_HOSTPKG)/bin
endif

NODE := $(NODE_PATH) node
NPM := $(NODE_PATH) npm

NODE_BIN := $(shell $(NODE_PATH) $(STAGING_DIR_HOST)/bin/which node)

ifneq ($(NODE_BIN),)
NODE_VER := $(shell $(NODE_BIN) -v | sed 's/v//')
endif

define CheckNode
	@echo
	@$(call ColorInfo, "Checking Node.js for building oui...")
	@if [ -n "$(CONFIG_OUI_USE_HOST_NODE)" ]; \
	then \
		$(call ColorInfo, "Using Node.js from Host"); \
	else \
		$(call ColorInfo, "Using Node.js from OpenWrt"); \
	fi
	@if [ -z "$(NODE_VER)" ]; \
	then \
		$(call ColorError, "Node.js $(1)+ is required"); \
		echo;false; \
	fi
	@$(call ColorInfo, "Node.js path: $(NODE_BIN)")
	@$(call ColorInfo, "Node.js version: $(NODE_VER)")
	@if [ "$(shell echo $(NODE_VER) | awk '{if ($$1 >= $(1)) { printf "ok" }}')" != "ok" ]; \
	then \
		$(call ColorError, "Node.js $(1)+ is required"); \
		echo;false; \
    fi
	@echo
endef
