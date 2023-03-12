/* SPDX-License-Identifier: MIT */
/*
 * Author: Jianhui Zhao <zhaojh329@gmail.com>
 */

import { reactive } from 'vue'
import * as Vue from 'vue'
import axios from 'axios'
import md5 from 'js-md5'
import i18n from '../i18n'

function mergeLocaleMessage(key, locales) {
  for (const locale in locales) {
    const messages = i18n.global.getLocaleMessage(locale)
    if (!messages.menus)
      messages.menus = {}
    messages.menus[key] = locales[locale]
  }
}

function getSID() {
  return sessionStorage.getItem('__oui__sid__') || ''
}

class Oui {
  constructor() {
    window.Vue = Vue
    this.menus = null
    this.inited = false
    this.aliveTimer = null
    this.state = reactive({
      sid: '',
      locale: '',
      theme: '',
      hostname: ''
    })

    const p = [
      this.call('ui', 'get_locale'),
      this.call('ui', 'get_theme')
    ]

    const sid = getSID()
    if (sid)
      p.push(this.rpc('alive', { sid }))

    Promise.all(p).then(results => {
      let locale = results[0].locale
      if (!locale)
        locale = 'auto'

      this.state.locale = locale

      if (locale === 'auto')
        i18n.global.locale = navigator.language
      else
        i18n.global.locale = locale

      this.state.theme = results[1].theme

      if (sid) {
        const alive = results[2].alive
        if (alive)
          this.initWithAlived(sid)
      }

      this.inited = true
    })
  }

  waitUntil(conditionFn) {
    if (conditionFn())
      return

    return new Promise((resolve) => {
      const intervalId = setInterval(() => {
        if (conditionFn()) {
          clearInterval(intervalId)
          resolve()
        }
      }, 10)
    })
  }

  initWithAlived(sid) {
    this.state.sid = sid

    this.ubus('system', 'board').then(({ hostname }) => this.state.hostname = hostname)

    this.aliveTimer = setInterval(() => {
      this.rpc('alive', { sid })
    }, 5000)
  }

  async rpc(method, params) {
    return (await axios.post('/oui-rpc', { method, params })).data
  }

  async call(mod, func, params = {}) {
    return (await this.rpc('call', [getSID(), mod, func, params])).result
  }

  ubus(obj, method, params) {
    return this.call('ubus', 'call', {object: obj, method, params})
  }

  reloadConfig(config) {
    return this.ubus('service', 'event', { type: 'config.change', data: { package: config }})
  }

  async login(username, password) {
    const { nonce } = await this.rpc('challenge', { username })
    const hash1 = md5(`${username}:${password}`)
    const hash2 = md5(`${hash1}:${nonce}`)
    const { sid } = await this.rpc('login', { username, password: hash2 })

    sessionStorage.setItem('__oui__sid__', sid)

    this.initWithAlived(sid)
  }

  logout() {
    this.menus = null
    const sid = getSID()
    if (!sid)
      return

    if (this.aliveTimer) {
      clearInterval(this.aliveTimer)
      this.aliveTimer = null
    }

    sessionStorage.removeItem('__oui__sid__')

    return this.rpc('logout', { sid })
  }

  async isAlived() {
    const sid = getSID()
    if (!sid)
      return false
    return (await this.rpc('alive', { sid })).alive
  }

  parseMenus(raw) {
    const menus = {}

    for (const path in raw) {
      const paths = path.split('/')
      if (paths.length === 2)
        menus[path] = raw[path]
    }

    for (const path in raw) {
      const paths = path.split('/')
      if (paths.length === 3) {
        const parent = menus['/' + paths[1]]
        if (!parent || parent.view)
          continue

        if (!parent.children)
          parent.children = {}

        parent.children[path] = raw[path]
        parent.children[path].parent = parent
      }
    }

    const menusArray = []

    for (const path in menus) {
      const m = menus[path]
      if (m.view || m.children) {
        menusArray.push({
          path: path,
          ...m
        })

        mergeLocaleMessage(m.title, m.locales)
      }
    }

    menusArray.sort((a, b) => (a.index ?? 0) - (b.index ?? 0))

    menusArray.forEach(m => {
      if (!m.children)
        return

      const children = []

      for (const path in m.children) {
        const c = m.children[path]
        children.push({
          path: path,
          ...c
        })

        mergeLocaleMessage(c.title, c.locales)
      }

      children.sort((a, b) => (a.index ?? 0) - (b.index ?? 0))

      m.children = children
    })

    return menusArray
  }

  async loadMenus() {
    if (this.menus)
      return this.menus
    const { menus } = await this.call('ui', 'get_menus')
    this.menus = this.parseMenus(menus)
    return this.menus
  }

  async setLocale(locale) {
    if (locale !== 'auto' && !i18n.global.availableLocales.includes(locale))
      return

    await this.call('uci', 'set', { config: 'oui', section: 'global', values: { locale }})

    this.state.locale = locale

    if (locale === 'auto')
      i18n.global.locale = navigator.language
    else
      i18n.global.locale = locale
  }

  async setTheme(theme) {
    await this.call('uci', 'set', { config: 'oui', section: 'global', values: { theme }})
    this.state.theme = theme
  }

  async setHostname(hostname) {
    await this.call('uci', 'set', { config: 'system', section: '@system[0]', values: { hostname }})
    await this.reloadConfig('system')
    this.state.hostname = hostname
  }

  reconnect(delay) {
    return new Promise(resolve => {
      let interval

      const img = document.createElement('img')

      img.addEventListener('load', () => {
        window.clearInterval(interval)
        img.remove()
        resolve()
      })

      window.setTimeout(() => {
        interval = window.setInterval(() => {
          img.src = '/favicon.ico?r=' + Math.random()
        }, 1000)
      }, delay || 5000)
    })
  }

  install(app) {
    app.config.globalProperties.$oui = this
    app.config.globalProperties.$md5 = md5
  }
}

export default new Oui()
