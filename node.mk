CL_RED=$(shell tput setaf 1)
CL_CYN=$(shell tput setaf 6)
CL_RST=$(shell tput sgr0)

define ColorInfo
  $(info $(CL_CYN)$1$(CL_RST))
endef

define ColorError
  $(error $(CL_RED)$1$(CL_RST))
endef

ifneq ($(CONFIG_OUI_USE_HOST_NODE),)
OUI_NODE_PATH := $(subst ",,$(CONFIG_OUI_HOST_NODE_PATH))
ifneq ($(OUI_NODE_PATH),)
OUI_NODE_PATH := PATH=$(OUI_NODE_PATH)
endif
NODE := $(OUI_NODE_PATH) node
NPM := $(OUI_NODE_PATH) npm
$(eval $(call ColorInfo,Using Node.js from Host))
else
NODE := node
NPM := npm
$(eval $(call ColorInfo,Using Node.js from OpenWrt))
endif

NODE_VER := $(shell $(NODE) -v | sed 's/v//' | awk -F. '{print $$1"."$$2}')

$(eval $(call ColorInfo,Node.js version: $(NODE_VER)))

ifneq ($(shell echo ${NODE_VER} | awk '{if ($$1 >= 14.18) { print "y" }}'),y)
$(eval $(call ColorError,Node.js 14.18+ is required))
endif
