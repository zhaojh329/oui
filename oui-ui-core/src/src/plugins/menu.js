import {ubus} from './ubus'

const menu = {}

function parseMenus(raw) {
  let menus = {};

  Object.keys(raw).forEach(m => {
    const paths = m.split('/');
    if (paths.length === 1)
      menus[paths[0]] = raw[m];
  });

  Object.keys(raw).forEach(m => {
    const [menu, submenu] = m.split('/');
    if (submenu && menus[menu]) {
      if (!menus[menu].children)
        menus[menu].children = {};
      menus[menu].children[submenu] = raw[m];
    }
  });

  menus = Object.keys(menus).map(k => {
    return {path: '/' + k, ...menus[k]};
  });

  menus.forEach(m => {
    if (!m.children)
      return;

    m.children = Object.keys(m.children).map(k => {
      return {path: `${m.path}/${k}`, ...m.children[k]};
    });

    m.children.sort((a, b) => a.index - b.index);
  });

  menus.sort((a, b) => a.index - b.index);

  return menus;
}

function buildRoutes(menus) {
  const routes = [];

  menus.forEach(menu => {
    const route = {
      path: '/',
      component: () => import('@/components/layout'),
      meta: {
        title: menu.title
      },
      children: []
    };

    if (menu.view) {
      route.redirect = menu.path;
      route.children.push({
        path: menu.path,
        component: () => import(`@/views/${menu.view}`),
        meta: {
          title: menu.title
        }
      });
    } else if (menu.children) {
      menu.children.forEach(sm => {
        route.children.push({
          path: sm.path,
          component: () => import(`@/views/${sm.view}`),
          meta: {
            title: sm.title
          }
        });
      });
    }

    routes.push(route);
  });

  return routes;
}

menu.load = function(cb) {
  ubus.call('oui.ui', 'menu').then(r => {
    const menus = parseMenus(r.menu);
    const routes = buildRoutes(menus);
    cb(menus, routes);
  });
}

export default {
  install(Vue) {
    Vue.prototype.$menu = menu;
  }
}
