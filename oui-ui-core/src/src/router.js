import Vue from 'vue'
import Router from 'vue-router'
import {session} from './plugins/session'
import {ubus} from './plugins/ubus'
import store from './store'
import i18n from './i18n'

Vue.use(Router)

const router = new Router({
  routes: [
    {
      path: '/login',
      component: () => import('@/views/login.vue')
    },
    {
      path: '/',
      redirect: '/home',
      component: () => import('@/components/layout'),
      children: [
        {
          path: 'home',
          component: () => import('@/views/home.vue'),
          meta:  {
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

function beforeEach(to, next) {
  if (to.path !== '/login') {
    session.isAlive().then(alive => {
      if (alive) {
        session.startHeartbeat();

        if (!session.aclCache) {
          session.updateACLs().then(() => {
            next();
          });
        } else {
          next();
        }
      } else {
        next('/login');
      }
    });
  } else {
    next();
  }
}

router.beforeEach((to, from, next) => {
  if (!store.state.lang) {
    ubus.call('oui.ui', 'lang').then(({lang}) => {
      store.commit('setLang', lang);
      if (lang === 'auto')
        lang = navigator.language;
      i18n.locale = lang;

      beforeEach(to, next);
    });
  } else {
    beforeEach(to, next);
  }
});

router.onError(err => {
  if (err.code !== 'MODULE_NOT_FOUND')
    throw err
  router.push('/404');
});

export default router
