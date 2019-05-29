'use strict'

import {ubus} from './ubus'

const network = {}

network.load = function() {
  return new Promise(resolve => {
    ubus.call('network.interface', 'dump').then(r => {
      this.interfaces = r.interface;
      resolve();
    });
  });
}

network.getInterface = function(name) {
  const interfaces = this.interfaces;

  for (let i = 0; i < interfaces.length; i++) {
    if (interfaces[i].name === name)
      return interfaces[i];
  }

  return null;
}

network.getInterfaces = function() {
  return this.interfaces;
}

export default {
  install(Vue) {
    Vue.prototype.$network = network;
  }
}
