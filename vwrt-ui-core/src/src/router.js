import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/login',
      component: () => import('@/views/Login.vue')
    },
    {
      path: '/',
      redirect: '/home',
      component: () => import('@/views/Main.vue'),
      children: [
        {
          path: 'home',
          component: () => import('@/views/Home.vue')
        }
      ]
    }
  ]
})
