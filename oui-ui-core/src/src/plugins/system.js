import {ubus} from './ubus'

export const system = {}

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
      resolve(Object.assign({}, r[0], r[1], {disk: r[2]}));
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

system.initWaitEnabled = function(name, expect, resolve) {
  function check() {
    this.initEnabled(name).then(enabled => {
      if (!!enabled === expect) {
        resolve();
        return;
      }

      setTimeout(() => {
        check.call(this);
      }, 500);
    });
  }

  check.call(this);
}

system.initEnable = function(name) {
  return new Promise(resolve => {
    this.initRun(name, 'enable').then(() => {
      this.initWaitEnabled(name, true, resolve);
    });
  });
}

system.initDisable = function(name) {
  return new Promise(resolve => {
    this.initRun(name, 'disable').then(() => {
      this.initWaitEnabled(name, false, resolve);
    });
  });
}

system.setPassword = function(user, password) {
  return ubus.call('rpc-sys', 'password_set', {user, password})
}

system.reboot = function() {
  return ubus.call('system', 'reboot')
}

export default {
  install(Vue) {
    Vue.prototype.$system = system;
  }
}
