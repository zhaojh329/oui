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

system.initList = function() {
  return new Promise(resolve => {
    ubus.call('oui.system', 'init_list').then(r => {
      resolve(r.initscripts);
    });
  });
}

system.initEnabled = function(name) {
  return new Promise(resolve => {
    this.initList().then(list => {
      for (let i = 0; i < list.length; i++) {
        if (list[i].name === name) {
          resolve(list[i].enabled);
          return;
        }
      }
      resolve(false);
    });
  });
}

system.initRun = function(name, action) {
  return ubus.call('oui.system', 'init_action', {name, action});
}

system.initStart = function(name) {
  return this.initRun(name, 'start');
}

system.initStop = function(name) {
  return this.initRun(name, 'stop');
}

system.initRestart = function(name) {
  return this.initRun(name, 'restart');
}

system.initEnable = function(name) {
  return this.initRun(name, 'enable');
}

system.initDisable = function(name) {
  return this.initRun(name, 'disable');
}

export default {
  install(Vue) {
    Vue.prototype.$system = system;
  }
}
