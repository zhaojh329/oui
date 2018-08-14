const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin
const address = require('address')

module.exports = {
  indexPath: 'vuci.html',
  productionSourceMap: false,
  devServer: {
    proxy: {
      '/ubus': {
        target: 'http://openwrt.lan'
      },
    }
  },
  configureWebpack: config => {
    if (process.env.ANALYZ) {
      let analyz = new BundleAnalyzerPlugin({
        analyzerHost: address.ip()
      });
      config.plugins.push(analyz)
    }
  }
}
