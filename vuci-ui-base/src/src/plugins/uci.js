import * as ubus from './ubus.js'

let state = {}

function load(config) {
  return new Promise(function(resolve, reject) {
    ubus.call('uci', 'get', {config: config}).then((r) => {
      state[config] = r.values;
      resolve(config);
    }).catch((r) => {
      reject(r);
    });
  });
}

function get(config, session, option) {
  let values = state[config];

  if (session) {
    values = values[session];
    if (option)
      return values[option];
  }
  return values;
}

const uci = {}

uci.install = function(Vue) {
  Vue.prototype.$uci = {
    load: load,
    get: get
  }
}

export default uci
