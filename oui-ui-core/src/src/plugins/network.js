'use strict'

import {ubus} from './ubus'

const network = {
  interfaces: [],
  devices: []
}

class Interface {
  constructor(iface) {
    this.status = iface;
    this.name = iface.interface;
  }

  isUp() {
    return this.status['up'];
  }

  getUptime() {
    const uptime = this.status['uptime'];
    return isNaN(uptime) ? 0 : uptime;
  }

  getDevice() {
    const l3dev = this.status['l3_device'];
    if (l3dev)
      return network.getDevice(l3dev);

    return undefined;
  }

  getAddrs(key, mask) {
    const rv = [];
    const addrs = this.status[key];

    if (addrs) {
      addrs.forEach(addr => {
        let address = addr.address;
        if (mask)
          address += `/${addr.mask}`;
        rv.push(address);
      });
    }

    return rv;
  }

  getIPv4Addrs(mask) {
    return this.getAddrs('ipv4-address', mask);
  }

  getIPv6Addrs(mask) {
    const rv = [];

    rv.push(...this.getAddrs('ipv6-address', mask))
    rv.push(...this.getAddrs('ipv6-prefix-assignment', mask))

    return rv;
  }

  getStatistics() {
    const dev = this.getDevice();
    if (dev)
      return dev.statistics;
    return {};
  }
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

      this.interfaces = r[1].interface.map(iface => new Interface(iface));

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
