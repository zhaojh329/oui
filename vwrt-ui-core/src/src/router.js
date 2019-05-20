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
      component: () => import('@/views/main.vue'),
      children: [
        {
          path: 'home',
          component: () => import('@/views/home.vue')
        }
      ]
    }
  ]
})

router.beforeEach((to, from, next) => {
  if (from.path === '/' && to.path !== '/home') {
    next('/');
    return;
  }

  if (to.path !== '/login') {
    session.isAlive().then(alive => {
      if (alive)
        next();
      else
        next('/login');
    });
  } else {
    next();
  }
});

export default router