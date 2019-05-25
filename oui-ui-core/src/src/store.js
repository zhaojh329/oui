import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    menus: []
  },
  mutations: {
    setMenus(state, menus) {
      state.menus = menus;
    }
  }
})
