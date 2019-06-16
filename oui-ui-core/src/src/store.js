import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    menus: [],
    hostname: '',
    lang: ''
  },
  mutations: {
    setMenus(state, menus) {
      state.menus = menus;
    },
    setLang(state, lang) {
      state.lang = lang;
    },
    setHostname(state, hostname) {
      state.hostname = hostname;
    }
  }
})
