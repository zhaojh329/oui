import Vue from 'vue'
import Router from 'vue-router'
import { session } from '@/plugins/session'
import { rpc } from '@/plugins/rpc'
import axios from 'axios'

Vue.use(Router)

const router = new Router({
  routes: [
    {
      path: '/wizard',
      component: resolve => {
        axios.get(`/views/oui-app-wizard.js?_t=${new Date().getTime()}`).then(r => {
          // eslint-disable-next-line no-eval
          return resolve(eval(r.data))
        }).catch(() => {
          return resolve(require('@/views/Wizard.vue'))
        })
      }
    },
    {
      path: '/login',
      component: resolve => {
        axios.get(`/views/oui-app-login.js?_t=${new Date().getTime()}`).then(r => {
          // eslint-disable-next-line no-eval
          return resolve(eval(r.data))
        }).catch(() => {
          return resolve(require('@/views/Login.vue'))
        })
      }
    },
    {
      path: '/',
      redirect: '/home',
      component: () => import('@/components/OuiLayout'),
      children: [
        {
          path: 'home',
          component: resolve => {
            axios.get(`/views/oui-app-home.js?_t=${new Date().getTime()}`).then(r => {
              // eslint-disable-next-line no-eval
              return resolve(eval(r.data))
            }).catch(() => {
              return resolve(require('@/views/Home.vue'))
            })
          },
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

function firstLogin () {
  return new Promise(resolve => {
    const value = sessionStorage.getItem('__oui_first_login')
    if (value === null) {
      rpc.call('ui', 'first_login').then(r => {
        sessionStorage.setItem('__oui_first_login', r.first)
        resolve(r.first)
      })
    } else {
      resolve(value === 'true')
    }
  })
}

router.beforeEach((to, from, next) => {
  firstLogin().then(first => {
    if (first) {
      if (to.path !== '/wizard') { next('/wizard') } else { next() }
    } else {
      if (to.path === '/wizard') {
        next('/')
        return
      }

      session.isAlive().then((alive) => {
        if (alive) session.startHeartbeat()
        else session.logout()

        beforeEach(to, next, alive)
      })
    }
  })
})

export default router
