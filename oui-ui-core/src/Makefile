NODE ?= node
NPM ?= npm

$(info node: $(shell $(NODE) -v))
$(info npm: $(shell $(NPM) -v))

node_ver = $(subst v,,$(shell $(NODE) -v))
node_ver_major = $(shell echo $(node_ver) | awk -F'.' '{print $$1}')
node_ver_minor = $(shell echo $(node_ver) | awk -F'.' '{print $$2}')

ifeq ($(node_ver),)
$(error "Node required at least v8.16, current not installed")
endif

ifeq ($(shell expr $(node_ver_major) \>= 8), 0)
$(error "Node required at least v8.16, current is $(node_ver)")
endif

ifeq ($(node_ver_major), 8)
ifeq ($(shell expr $(node_ver_minor) \>= 11), 0)
$(error "Node required at least v8.16, current is $(node_ver)")
endif
endif

all: build

.tmp/.npm_stamp: package.json
	@$(NPM) install
	@mkdir -p .tmp
	@touch .tmp/.npm_stamp

prepare: .tmp/.npm_stamp

build: prepare
	@$(NPM) run build

plugin:
	mv babel.config.js babel.config.js.bk
	@$(NPM) run plugin
	mv babel.config.js.bk babel.config.js
