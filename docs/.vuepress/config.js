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
          { text: 'uci', link: '/uci/' }
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
      '/guide/': ['', 'getting-started'],
      '/en/guide/': ['', 'getting-started'],
      '/uci/': ['', 'components'],
      '/en/uci/': ['', 'components']
    }
  }
}
