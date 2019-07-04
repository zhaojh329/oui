module.exports = {
  base: '/oui/',
  title: 'oui',
  locales: {
    '/': {
      lang: 'Englih'
    },
    '/zh/': {
      lang: '简体中文'
    }
  },
  themeConfig: {
    repo: 'zhaojh329/oui',
    locales: {
      '/': {
        selectText: 'Languages',
        nav: [
          { text: 'Guide', link: '/guide/' },
          { text: 'uci', link: '/uci/' }
        ]
      },
      '/zh/': {
        selectText: '语言',
        nav: [
          { text: '指南', link: '/zh/guide/' },
          { text: 'uci', link: '/zh/uci/' },
          { text: '中国站点(gitee)', link: 'https://zhaojh329.gitee.io/oui/' }
        ]
      }
    },
    sidebar: {
      '/guide/': ['', 'compile', 'getting-started'],
      '/zh/guide/': ['', 'compile', 'getting-started'],
      '/uci/': ['', 'components'],
      '/zh/uci/': ['', 'components']
    }
  }
}
