import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import './plugins/iview.js'
import axios from 'axios'
import VueAxios from 'vue-axios'
import VueTimers from 'vue-timers'
import helper from './plugins/helper'
import ubus from './plugins/ubus'
import menu from './plugins/menu'
import uci from './plugins/uci'
import system from './plugins/system'
import session from './plugins/session'
import CardList from './components/CardList.vue'
import CardTable from './components/CardTable.vue'
import UciMap from './components/UciMap.vue'
import UciSection from './components/UciSection.vue'
import UciValue from './components/UciValue.vue'
import UciInputValue from './components/UciInputValue.vue'
import UciListValue from './components/UciListValue.vue'

Vue.config.productionTip = false

Vue.use(VueAxios, axios)
Vue.use(VueTimers)

Vue.use(ubus)
Vue.use(menu)
Vue.use(uci)
Vue.use(system)
Vue.use(session)
Vue.use(helper)

Vue.component('CardList', CardList)
Vue.component('CardTable', CardTable)
Vue.component('UciMap', UciMap)
Vue.component('UciSection', UciSection)
Vue.component('UciValue', UciValue)
Vue.component('UciInputValue', UciInputValue)
Vue.component('UciListValue', UciListValue)

new Vue({
  router,
  store,
  render: h => h(App)
}).$mount('#app')
