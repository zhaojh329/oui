const CompressionPlugin = require('compression-webpack-plugin')

module.exports = {
  indexPath: 'oui.html',
  productionSourceMap: false,
  devServer: {
    proxy: {
      '/rpc': {
        target: 'http://openwrt.lan'
      },
      '/views/': {
        target: 'http://openwrt.lan'
      },
      '/i18n/': {
        target: 'http://openwrt.lan'
      },
      '/upload': {
        target: 'http://openwrt.lan'
      },
      '/download': {
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
  configureWebpack: config => {
    if (process.env.NODE_ENV === 'production') {
      return {
        plugins: [new CompressionPlugin({
          test: /\.js$|\.html$|\.css/,
          threshold: 4096,
          deleteOriginalAssets: true
        })]
      }
    }
  }
}
