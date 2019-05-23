all: build

.tmp/.npm_stamp: package.json
	npm install
	mkdir -p .tmp
	touch .tmp/.npm_stamp

prepare: .tmp/.npm_stamp

build: prepare
	npm run build
