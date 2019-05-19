"use strict";

import {ubus} from './ubus'
import router from '../router'

const session = { }

session.login = function(username, password) {
  sessionStorage.removeItem('sid');

  return new Promise((resolve) => {
    if (!password)
    password = '';

    ubus.call('session', 'login', {username, password}).then(r => {
      sessionStorage.setItem('sid', r.ubus_rpc_session);
      sessionStorage.setItem('username', r.data.username);
      this.startHeartbeat();
      resolve(true);
    }).catch(() => {
      resolve(false);
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
        sessionStorage.removeItem('sid');
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