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
      component: () => import('@/components/Main.vue'),
      children: [
        {
          path: 'home',
          component: () => import('@/views/home.vue')
        }
      ]
    },
    {
      path: '*',
      component: () => import('@/views/404.vue')
    }
  ]
})

router.beforeEach((to, from, next) => {
  if (to.path !== '/login') {
    session.isAlive().then(alive => {
      if (alive) {
        session.startHeartbeat();
        next();
      } else {
        next('/login');
      }
    });
  } else {
    next();
  }
});

export default router
