'use strict'

import {ubus} from './ubus'

const menu = {}

function parseMenus(raw) {
  let menus = {};

  Object.keys(raw).map(key => {
    if (key.indexOf('/') < 0) {
      menus[key] = Object.assign({children: []}, raw[key]);
      delete(raw[key]);
    }
  });

  Object.keys(raw).map(key => {
    const paths = key.split('/');
    raw[key].path = '/' + key;

    menus[paths[0]].children.push(raw[key]);
  });

  menus = Object.keys(menus).map(k => Object.assign({path: '/' + k}, menus[k]));

  menus.sort((a, b) => a.index - b.index);

  menus.forEach(e => {
    e.children.sort((a, b) => a.index - b.index);
  });

  return menus;
}

function buildRoutes(menus) {
  const routes = [];

  menus.forEach(e => {
    const route = {
      path: e.path,
      component: () => import('@/components/Main.vue'),
      children: []
    };

    e.children.forEach(c => {
      route.children.push({
        path: c.path,
        component: () => import(`@/views/${c.view}.vue`),
        meta: {
          title: c.title,
          parentTitle: e.title
        }
      });
    });
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
