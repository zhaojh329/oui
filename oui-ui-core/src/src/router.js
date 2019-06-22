import Vue from 'vue'
import Router from 'vue-router'
import {session} from './plugins/session'

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

router.beforeEach((to, from, next) => {
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
});

router.onError(err => {
  if (err.code !== 'MODULE_NOT_FOUND')
    throw err
  router.push('/404');
});

export default router
