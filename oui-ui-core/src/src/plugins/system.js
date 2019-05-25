'use strict'

import {ubus} from './ubus'

const system = {}

system.getSystemInfo = function() {
  return new Promise(resolve => {
    ubus.call('system', 'info').then(r => {
      resolve(r);
    });
  });
}

system.getBoardInfo = function() {
  return new Promise(resolve => {
    ubus.call('system', 'board').then(r => {
      resolve(r);
    });
  });
}

system.getDiskInfo = function() {
  return new Promise(resolve => {
    ubus.call('oui.system', 'diskfree').then(r => {
      resolve(r);
    });
  });
}

system.getInfo = function() {
  return new Promise(resolve => {
    ubus.callBatch([
      ['system', 'info'],
      ['system', 'board'],
      ['oui.system', 'diskfree']
    ]).then(r => {
      resolve(r);
    });
  });
}

export default {
  install(Vue) {
    Vue.prototype.$system = system;
  }
}
