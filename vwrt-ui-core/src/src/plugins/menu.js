"use strict";

const menu = {
}

menu.parse = function(raw) {
  return new Promise((resolve) => {
    console.log(raw);

    const pm = {};

    Object.keys(raw).map(key => {
      if (key.indexOf('/') < 0) {
        pm[key] = Object.assign({children: {}}, raw[key]);
        delete(raw[key]);
      }
    });

    Object.keys(raw).map(key => {
      const paths = key.split('/');
      pm[paths[0]].children[paths[1]] = raw[key];
    });

    console.log(pm);
    resolve();
  });
}

export default {
  install(Vue) {
    Vue.prototype.$menu = menu;
  }
}