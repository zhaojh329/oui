const Timestamp = new Date().getTime(); // 用当前时间作为标签，区分每次打包输出的文件，从而防止新打包的文件被客户端缓存

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
    output: { // 输出重构打包编译后的文件名，增加 Timestamp，从而防止新打包的文件被客户端缓存
      filename: `js/[name].[chunkhash:8].${Timestamp}.js`,
      chunkFilename: `js/[name].[chunkhash:8].${Timestamp}.js`
    }
  }
}
