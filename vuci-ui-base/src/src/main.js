import '@babel/polyfill'
import Vue from 'vue'
import axios from 'axios'
import VueAxios from 'vue-axios'
import App from './App.vue'
import router from './router'
import './material-design-icons/material-icons.css'

import './plugins/vuetify'
import VueFilter from './plugins/vue-filter.js'
import ubus from './plugins/ubus.js'
import session from './plugins/session.js'
import system from './plugins/system.js'
import uci from './plugins/uci.js'
import VueBus from './plugins/vue-bus.js'

import RebootDialog from './components/RebootDialog'
import CardTable from './components/CardTable.vue'
import CardList from './components/CardList.vue'

Vue.config.productionTip = false

Vue.use(VueAxios, axios)
Vue.use(VueFilter)
Vue.use(ubus)
Vue.use(session)
Vue.use(system)
Vue.use(uci)
Vue.use(VueBus)

Vue.use(RebootDialog)
Vue.component('card-table', CardTable)
Vue.component('card-list', CardList)

router.beforeEach((to, from, next) => {
  if (to.path === '/login')
    sessionStorage.removeItem('_ubus_rpc_session');

  if (!sessionStorage.getItem('_ubus_rpc_session') && to.path !== '/login')
    next('/login');
  else
    next();
});

new Vue({
  router,
  render: h => h(App)
}).$mount('#app')
