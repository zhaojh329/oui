"use strict";

import {ubus} from './ubus'
import router from '../router'

export const session = {
  sid: '00000000000000000000000000000000'
}

session.login = function(username, password) {
  this.sid = '00000000000000000000000000000000';

  return new Promise((resolve) => {
    if (!password)
    password = '';

    ubus.call('session', 'login', {username, password}).then(r => {
      this.sid = r.ubus_rpc_session;
      this.startHeartbeat();
      resolve(this.sid);
    }).catch(() => {
      resolve();
    });
  }); 
}

session.access = function(scope, object, fun) {
  return new Promise((resolve) => {
    ubus.call('session', 'access', {scope, object, function: fun}).then(r => {
      resolve(r.access);
    }).catch(() => {
      resolve(false);
    });
  }); 
}

session.isAlive = function() {
  return this.access('ubus', 'session', 'access');
}

session.startHeartbeat = function() {
  this._hearbeatInterval = window.setInterval(() => {
    this.isAlive().then(alive => {
      if (!alive) {
        this.stopHeartbeat();
        router.push('/login');
      }
    });
  }, 15000);
}

session.stopHeartbeat = function() {
  if (typeof(this._hearbeatInterval) != 'undefined') {
    window.clearInterval(this._hearbeatInterval);
    delete this._hearbeatInterval;
  }
}

export default {
  install(Vue) {
    Vue.prototype.$session = session;
  }
}