const CompressionPlugin = require('compression-webpack-plugin')
const path = require('path')

module.exports = {
  productionSourceMap: false,
  css: {
    extract: false
  },
  configureWebpack: config => {
    if (process.env.NODE_ENV === 'production') {
      return {
        plugins: [new CompressionPlugin({
          test: /\.js$|\.html$|\.css/,
          threshold: 4096
        })]
      }
    }
  }
}
