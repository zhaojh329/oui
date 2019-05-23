module.exports = {
    indexPath: 'oui.html',
    productionSourceMap: false,
    devServer: {
        proxy: {
            '/ubus': {
                target: 'http://openwrt.lan'
            }
        }
    }
}
