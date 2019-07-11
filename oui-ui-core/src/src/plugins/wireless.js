import {ubus} from './ubus'

const wireless = {}

wireless.getDevices = function() {
  return new Promise(resolve => {
    if (this.devices) {
      resolve(this.devices);
      return;
    }

    ubus.call('iwinfo', 'devices').then(r => {
      this.devices = r.devices;
      resolve(this.devices);
    });
  });
}

wireless.getAssoclist = function() {
  return new Promise(resolve => {
    this.getDevices().then(devices => {
      const batch = [];

      if (devices.length < 1) {
        resolve([]);
        return;
      }

      devices.forEach(dev => {
        batch.push(['iwinfo', 'assoclist', {device: dev}]);
      });

      ubus.callBatch(batch).then(rs => {
        const assoclist = [];

        rs.forEach(r => {
          assoclist.push(...r.results);
        });

        resolve(assoclist);
      });
    });
  });
}

export default {
  install(Vue) {
    Vue.prototype.$wireless = wireless;
  }
}
