const CompressionPlugin = require('compression-webpack-plugin')

module.exports = {
  indexPath: 'oui.html',
  productionSourceMap: false,
  devServer: {
    proxy: {
      '/rpc': {
        target: 'http://localhost'
      },
      '/views/': {
        target: 'http://localhost'
      },
      '/i18n/': {
        target: 'http://localhost'
      },
      '/upload': {
        target: 'http://localhost'
      },
      '/download': {
        target: 'http://localhost'
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
