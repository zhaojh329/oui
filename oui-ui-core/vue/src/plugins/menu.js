import { rpc } from './rpc'
import axios from 'axios'

const menu = {}

function parseMenus (raw) {
  let menus = {}

  Object.keys(raw).forEach(m => {
    const paths = m.split('/')
    if (paths.length === 1) { menus[paths[0]] = raw[m] }
  })

  Object.keys(raw).forEach(m => {
    const [menu, submenu] = m.split('/')
    if (submenu && menus[menu]) {
      if (!menus[menu].children) { menus[menu].children = {} }
      menus[menu].children[submenu] = raw[m]
    }
  })

  menus = Object.keys(menus).map(k => {
    return { path: '/' + k, ...menus[k] }
  })

  menus.forEach(m => {
    if (!m.children) { return }

    m.children = Object.keys(m.children).map(k => {
      return { path: `${m.path}/${k}`, ...m.children[k] }
    })

    m.children.sort((a, b) => a.index - b.index)
  })

  menus.sort((a, b) => a.index - b.index)

  return menus.filter(m => m.children)
}

function buildRoute (menu) {
  return {
    path: menu.path,
    component: resolve => {
      try {
        return resolve(require(`@/views/${menu.view}`))
      } catch {
        axios.get(`/views/${menu.view}.js?_t=${new Date().getTime()}`).then(r => {
          // eslint-disable-next-line no-eval
          return resolve(eval(r.data))
        }).catch(() => {
          return resolve(require('@/components/404.vue'))
        })
      }
    },
    meta: {
      title: menu.title
    }
  }
}

function buildRoutes (menus) {
  const routes = []

  menus.forEach(menu => {
    const route = {
      path: '/',
      component: () => import('@/components/OuiLayout'),
      meta: {
        title: menu.title
      },
      children: []
    }

    if (menu.view) {
      route.redirect = menu.path
      route.children.push(buildRoute(menu))
    } else if (menu.children) {
      menu.children.forEach(sm => {
        route.children.push(buildRoute(sm))
      })
    }

    routes.push(route)
  })

  return routes
}

menu.load = function (cb) {
  rpc.call('oui', 'menu').then(r => {
    const menus = parseMenus(r.menu)
    const routes = buildRoutes(menus)
    cb(menus, routes)
  })
}

export default {
  install (Vue) {
    Vue.prototype.$menu = menu
  }
}
