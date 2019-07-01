module.exports = {
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
        ]
      },
      '/en/': {
        selectText: 'Languages',
        nav: [
          { text: 'Guide', link: '/guide/' },
        ]
      }
    },
    sidebar: {
      '/guide/': ['', 'getting-started'],
      '/en/guide/': ['', 'getting-started']
    }
  }
}
