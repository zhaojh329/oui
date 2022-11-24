ifneq ($(CONFIG_OUI_USE_HOST_NODE),)
	ifeq ($(shell uname -s), Darwin)
		ifeq ($(shell uname -m), arm64)
			NPM := PATH=/opt/homebrew/bin/:/bin npm
		else
			NPM := PATH=/usr/local/bin:/bin npm
		endif
	else
		NPM := PATH=/usr/local/bin:/usr/bin npm
	endif
else
	NPM := npm
endif
