import Vue from 'vue'
import Vuex from 'vuex'
import {system} from './plugins/system'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    menus: [],
    hostname: ''
  },
  mutations: {
    setMenus(state, menus) {
      state.menus = menus;
    },
    updateData(state) {
      system.getBoardInfo().then(r => {
        state.hostname = r.hostname;
      });
    }
  },
  actions: {
    updateData({ commit }) {
      setTimeout(() => {
        commit('updateData');
      }, 2000);
    }
  }
})
