import Vue from 'vue'
import VueTimers from 'vue-timers'
import fullscreen from 'vue-fullscreen'
import Antd from 'ant-design-vue'
import App from './App.vue'
import 'ant-design-vue/dist/antd.css'
import './plugins/oui'
import router from './router'
import store from './store'
import i18n from './i18n'
import './plugins/debug'
import './plugins/string-format'

import prompt from './plugins/prompt'
import rpc from './plugins/rpc'
import helper from './plugins/helper'
import menu from './plugins/menu'
import uci from './plugins/uci'
import system from './plugins/system'
import session from './plugins/session'
import network from './plugins/network'
import firewall from './plugins/firewall'
import wireless from './plugins/wireless'

import OuiLine from '@/components/OuiLine.vue'
import OuiDashboard from '@/components/OuiDashboard.vue'

import '@/components/OuiForm'

Vue.config.productionTip = false

Vue.use(VueTimers)

Vue.use(fullscreen)

Vue.use(Antd)

Vue.use(prompt)
Vue.use(rpc)
Vue.use(session)
Vue.use(menu)
Vue.use(uci)
Vue.use(system)
Vue.use(session)
Vue.use(helper)
Vue.use(network)
Vue.use(firewall)
Vue.use(wireless)

Vue.component('OuiLine', OuiLine)
Vue.component('OuiDashboard', OuiDashboard)

new Vue({
  router,
  store,
  i18n,
  render: (h) => h(App)
}).$mount('#app')
