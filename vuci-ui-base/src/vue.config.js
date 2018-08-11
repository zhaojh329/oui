module.exports = {
  indexPath: 'vuci.html',
  productionSourceMap: false,
  devServer: {
    proxy: {
      '/ubus': {
        target: 'http://openwrt.lan'
      },
    }
  }
}
