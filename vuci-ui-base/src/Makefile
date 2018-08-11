all: build

.tmp/.download_stamp: package.json
	if [ -n "$(USE_TAOBAO_REGISTRY)" ];then \
		npm --registry=https://registry.npm.taobao.org install; \
	else \
		npm install; \
	fi
	mkdir -p .tmp
	touch .tmp/.download_stamp

prepare: .tmp/.download_stamp

build: prepare
	npm run build
