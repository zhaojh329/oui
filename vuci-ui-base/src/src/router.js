import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/login',
      component: resolve => require(['@/views/Login'], resolve)
    },
    {
      path: '/',
      redirect: '/status/overview',
      component: resolve => require(['@/views/Home'], resolve),
      children: [
        {
          path: '/status/overview',
          component: resolve => require(['@/views/status/Overview'], resolve)
        },
        {
          path: '/status/routes',
          component: resolve => require(['@/views/status/Routes'], resolve)
        },
        {
          path: '/status/syslog',
          component: resolve => require(['@/views/status/Syslog'], resolve)
        },
        {
          path: '/status/dmesg',
          component: resolve => require(['@/views/status/Dmesg'], resolve)
        },
        {
          path: '/status/processes',
          component: resolve => require(['@/views/status/Processes'], resolve)
        }
      ]
    }
  ]
})
