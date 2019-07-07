import {ubus} from './ubus'

const wireless = {}

wireless.getAssoclist = function() {
  return new Promise(resolve => {
    ubus.call('iwinfo', 'assoclist', {device: 'wlan1'}).then(r => {
      resolve(r.results);
    });
  });
}

export default {
  install(Vue) {
    Vue.prototype.$wireless = wireless;
  }
}
