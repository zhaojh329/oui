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

function beforeEach(to, next, alive) {
  if (to.path !== '/login') {
    if (alive) {
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
  } else {
    next();
  }
}

router.beforeEach((to, from, next) => {
  session.isAlive().then(alive => {
    if (alive)
      session.startHeartbeat();
    else
      session.logout();

    if (!store.state.lang) {
      ubus.call('oui.ui', 'lang').then(({lang}) => {
        store.commit('setLang', lang);
        if (lang === 'auto')
          lang = navigator.language;
        i18n.locale = lang;

        beforeEach(to, next, alive);
      });
    } else {
      beforeEach(to, next, alive);
    }
  });
});

router.onError(err => {
  if (err.code !== 'MODULE_NOT_FOUND')
    throw err
  router.push('/404');
});

export default router
