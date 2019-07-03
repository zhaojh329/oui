module.exports = {
  base: '/oui/',
  title: 'oui',
  locales: {
    '/': {
      lang: '简体中文'
    },
    '/en/': {
      lang: 'Englih'
    }
  },
  themeConfig: {
    repo: 'zhaojh329/oui',
    locales: {
      '/': {
        selectText: '语言',
        nav: [
          { text: '指南', link: '/guide/' },
          { text: 'uci', link: '/uci/' },
          { text: '中文站点(gitee)', link: 'https://zhaojh329.gitee.io/oui/' }
        ]
      },
      '/en/': {
        selectText: 'Languages',
        nav: [
          { text: 'Guide', link: '/en/guide/' },
          { text: 'uci', link: '/en/uci/' }
        ]
      }
    },
    sidebar: {
      '/guide/': ['', 'getting-started', 'compile'],
      '/en/guide/': ['', 'getting-started', 'compile'],
      '/uci/': ['', 'components'],
      '/en/uci/': ['', 'components']
    }
  }
}
