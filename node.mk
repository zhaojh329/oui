ifneq ($(CONFIG_OUI_USE_HOST_NODE),)
NPM := PATH=/usr/local/bin:/usr/bin npm
else
NPM := npm
endif
