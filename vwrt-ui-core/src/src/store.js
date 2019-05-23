import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    username: '',
    menus: []
  },
  mutations: {
    setUsername(state, username) {
      state.username = username;
    },
    setMenus(state, menus) {
      state.menus = menus;
    }
  },
  actions: {

  }
})
