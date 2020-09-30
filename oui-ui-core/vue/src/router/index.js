import Vue from 'vue'
import Router from 'vue-router'
import { session } from '@/plugins/session'

Vue.use(Router)

const router = new Router({
  routes: [
    {
      path: '/login',
      component: () => import('@/views/Login.vue')
    },
    {
      path: '/',
      redirect: '/home',
      component: () => import('@/components/OuiLayout'),
      children: [
        {
          path: 'home',
          component: () => import('@/views/Home.vue'),
          meta: {
            title: 'Home'
          }
        }
      ]
    },
    {
      path: '*',
      component: () => import('@/components/404.vue')
    }
  ]
})

function beforeEach (to, next, alive) {
  if (to.path !== '/login') {
    if (alive) {
      next()
    } else {
      next('/login')
    }
  } else {
    next()
  }
}

router.beforeEach((to, from, next) => {
  session.isAlive().then((alive) => {
    if (alive) session.startHeartbeat()
    else session.logout()

    beforeEach(to, next, alive)
  })
})

export default router
