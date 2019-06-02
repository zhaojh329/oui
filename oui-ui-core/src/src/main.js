import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
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

import CardList from './components/CardList.vue'
import CardTable from './components/CardTable.vue'

import UciForm from './components/UciForm.vue'
import UciTabs from './components/UciTabs.vue'
import UciSection from './components/UciSection.vue'
import UciInput from './components/UciInput.vue'
import UciList from './components/UciList.vue'
import UciSwitch from './components/UciSwitch.vue'

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

Vue.component('UciForm', UciForm)
Vue.component('UciTabs', UciTabs)
Vue.component('UciSection', UciSection)
Vue.component('UciInput', UciInput)
Vue.component('UciList', UciList)
Vue.component('UciSwitch', UciSwitch)

new Vue({
  router,
  store,
  render: h => h(App)
}).$mount('#app')
