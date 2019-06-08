'use strict'

import {ubus} from './ubus'

const network = {
  interfaces: [],
  devices: []
}

network.load = function() {
  const promises = [];

  promises.push(ubus.call('network.device', 'status'));
  promises.push(ubus.call('network.interface', 'dump'));

  return new Promise(resolve => {
    Promise.all(promises).then(r => {
      const devices = r[0];
      this.devices = [];
      Object.keys(devices).forEach(name => {
        this.devices.push({name: name, ...devices[name]});
      });

      this.interfaces = r[1].interface;

      resolve();
    });
  });
}

network.getInterface = function(name) {
  const interfaces = this.interfaces;

  for (let i = 0; i < interfaces.length; i++) {
    if (interfaces[i].interface === name)
      return interfaces[i];
  }

  return null;
}

network.getInterfaces = function() {
  return this.interfaces;
}

network.getDevice = function(name) {
  const devices = this.devices;

  for (let i = 0; i < devices.length; i++) {
    if (devices[i].name === name)
      return devices[i];
  }

  return null;
}

network.getDevices = function() {
  return this.devices;
}

export default {
  install(Vue) {
    Vue.prototype.$network = network;
  }
}
