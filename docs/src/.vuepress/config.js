const { defaultTheme } = require('vuepress')

module.exports = {
  base: '/oui/',
  lang: 'zh-CN',
  title: 'Nginx + Lua + Vue3 = Oui',
  description: '一个用于开发 OpenWrt Web 接口的框架',
  theme: defaultTheme({
    repo: 'zhaojh329/oui',
    locales: {
      '/': {
        editLinkText: '在 GitHub 上编辑此页',
        lastUpdatedText: '上次更新',
        contributorsText: '贡献者',
        tip: '提示',
        warning: '注意',
        danger: '警告'
      }
    }
  })
}
