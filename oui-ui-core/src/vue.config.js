const Timestamp = new Date().getTime();

module.exports = {
  indexPath: 'oui.html',
  productionSourceMap: false,
  devServer: {
    proxy: {
      '/ubus': {
        target: 'http://openwrt.lan'
      },
      '/cgi-bin/': {
        target: 'http://openwrt.lan'
      },
      '/views/': {
        target: 'http://openwrt.lan'
      },
      '/i18n/': {
        target: 'http://openwrt.lan'
      }
    }
  },
  pluginOptions: {
    i18n: {
      locale: 'en',
      fallbackLocale: 'en',
      localeDir: 'locales',
      enableInSFC: false
    }
  },
  configureWebpack: {
    output: {
      filename: `js/[name].[hash:8].${Timestamp}.js`,
      chunkFilename: `js/[name].[hash:8].${Timestamp}.js`
    }
  }
}
