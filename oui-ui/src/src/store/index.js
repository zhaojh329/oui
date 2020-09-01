import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    menus: [],
    hostname: '',
    lang: '',
    fullscreen: false,
    spinning: 0,
    spintip: null
  },
  mutations: {
    setMenus (state, menus) {
      state.menus = menus
    },
    setLang (state, lang) {
      state.lang = lang
    },
    setHostname (state, hostname) {
      state.hostname = hostname
    },
    toggleFullscreen (state) {
      state.fullscreen = !state.fullscreen
    },
    spin (state, config) {
      if (typeof config === 'boolean') {
        config = {
          show: config
        }
      } else if (typeof config === 'string') {
        config = {
          show: true,
          tip: config
        }
      }

      config = config || { show: true }

      if (typeof config.show === 'undefined') { config.show = true }

      if (typeof config.tip === 'string') { state.spintip = config.tip } else { state.spintip = null }

      if (config.show) { state.spinning++ } else { state.spinning-- }
    }
  }
})
