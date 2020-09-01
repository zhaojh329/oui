import Vue from 'vue'
import Router from 'vue-router'
import { session } from '@/plugins/session'
import { rpc } from '@/plugins/rpc'
import store from '../store'
import i18n from '../i18n'

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

    if (!store.state.lang) {
      rpc.call('oui', 'lang').then(({ lang }) => {
        store.commit('setLang', lang)
        if (lang === 'auto') i18n.locale = navigator.language
        else i18n.locale = lang

        beforeEach(to, next, alive)
      })
    } else {
      beforeEach(to, next, alive)
    }
  })
})

router.onError((err) => {
  if (err.code !== 'MODULE_NOT_FOUND') throw err
  router.push('/404').then()
})

export default router
