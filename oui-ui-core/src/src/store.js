import Vue from 'vue'
import Vuex from 'vuex'
import {system} from './plugins/system'

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
