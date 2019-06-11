import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import i18n from './i18n'
import './plugins/element.js'
import './assets/iconfont/iconfont.css'
import axios from 'axios'
import VueAxios from 'vue-axios'
import VueTimers from 'vue-timers'
import helper from './plugins/helper'
import ubus from './plugins/ubus'
import menu from './plugins/menu'
import uci from './plugins/uci'
import system from './plugins/system'
import session from './plugins/session'
import network from './plugins/network'

import CardList from './components/card-list.vue'
import CardTable from './components/card-table.vue'

import './components/uci-form'

Vue.config.productionTip = false

Vue.use(VueAxios, axios)
Vue.use(VueTimers)

Vue.use(ubus)
Vue.use(menu)
Vue.use(uci)
Vue.use(system)
Vue.use(session)
Vue.use(helper)
Vue.use(network)

Vue.component('CardList', CardList)
Vue.component('CardTable', CardTable)

new Vue({
  router,
  store,
  i18n,
  render: h => h(App)
}).$mount('#app')
