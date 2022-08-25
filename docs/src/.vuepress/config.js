const { registerComponentsPlugin } = require('@vuepress/plugin-register-components')
const { defaultTheme } = require('vuepress')
const { path } = require('@vuepress/utils')

module.exports = {
  base: '/oui/',
  lang: 'zh-CN',
  title: 'Oui',
  description: '一个用于开发 OpenWrt Web 接口的框架',
  theme: defaultTheme({
    repo: 'zhaojh329/oui',
    docsBranch: 'master',
    docsDir: 'docs/src',
    locales: {
      '/': {
        editLinkText: '在 GitHub 上编辑此页',
        lastUpdatedText: '上次更新',
        contributorsText: '贡献者',
        tip: '提示',
        warning: '注意',
        danger: '警告',
        navbar: [
          {
            text: '指南',
            link: '/guide/'
          }
        ],
        sidebar: {
          '/guide/': [
            {
              text: '指南',
              children: [
                '/guide/README.md',
                '/guide/getting-started.md',
                '/guide/page.md',
                '/guide/vue-api.md',
                '/guide/lua-lib.md'
              ]
            }
          ]
        }
      }
    }
  }),
  plugins: [
    registerComponentsPlugin({
      componentsDir: path.resolve(__dirname, './components')
    })
  ]
}
