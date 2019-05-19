import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import './plugins/iview.js'
import axios from 'axios'
import VueAxios from 'vue-axios'
import ubus from './plugins/ubus'
import session from './plugins/session'
import CardList from './components/CardList.vue'
import CardTable from './components/CardTable.vue'

Vue.config.productionTip = false

Vue.use(VueAxios, axios)
Vue.use(ubus)
Vue.use(session)

Vue.component('CardList', CardList)
Vue.component('CardTable', CardTable)

new Vue({
  router,
  store,
  render: h => h(App)
}).$mount('#app')
