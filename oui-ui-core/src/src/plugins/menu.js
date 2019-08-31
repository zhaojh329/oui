import {ubus} from './ubus'
import axios from 'axios'
import i18n from '@/i18n'

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

function buildRoute(menu) {
  if (menu.i18n) {
    const msgs = menu.i18n;
    for (const locale in msgs)
      i18n.mergeLocaleMessage(locale, msgs[locale]);
  }

  return {
    path: menu.path,
    component: resolve => {
      if (menu.plugin) {
        axios.get(`/views/${menu.view}.js`).then(r => {
          resolve(eval(r.data));
        });
      } else {
        return import(`@/views/${menu.view}`);
      }
    },
    meta: {
      title: menu.title,
      i18nfile: menu.i18nfile
    },
    beforeEnter: (to, from, next) => {
      if (!to.meta.i18nfile) {
        next();
        return;
      }

      to.meta.i18nfile = false;

      axios.get(`/i18n${to.path}.json`).then(r => {
        const msgs = r.data;
        for (const locale in msgs)
          i18n.mergeLocaleMessage(locale, msgs[locale]);
        next();
      }).catch(() => {
        next();
      });
    }
  };
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
      route.children.push(buildRoute(menu));
    } else if (menu.children) {
      menu.children.forEach(sm => {
        route.children.push(buildRoute(sm));
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
