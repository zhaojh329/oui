module.exports = {
    indexPath: 'vwrt.html',
    productionSourceMap: false,
    devServer: {
        proxy: {
            '/ubus': {
                target: 'http://openwrt.lan'
            }
        }
    }
}
