import Vue from 'vue'
import Router from 'vue-router'

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

router.beforeEach((to, from, next) => {
  if (!sessionStorage.getItem('sid') && to.path !== '/login')
    next('/login');
  else
    next();
});

export default router