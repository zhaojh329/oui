import {ubus} from './ubus'
import {uci} from './uci'

const DEFAULT_SESSION_ID = '00000000000000000000000000000000'

export const session = {
  aclCache: null
}

session.sid = function() {
  return sessionStorage.getItem('sid') || DEFAULT_SESSION_ID;
}

session.login = function(username, password) {
  return new Promise(resolve => {
    if (!password) {
      password = '';
    }

    ubus.call('session', 'login', {username, password}).then(r => {
      this.startHeartbeat();
      sessionStorage.setItem('sid', r.ubus_rpc_session);
      resolve(true);
    }).catch(() => {
      resolve(false);
    });
  });
}

session.logout = function() {
  return new Promise(resolve => {
    ubus.call('session', 'destroy').then(() => {
      resolve();
    }).catch(() => {
      resolve();
    });

    this.stopHeartbeat();
    sessionStorage.removeItem('sid');
  });
}

session.access = function(scope, object, fun) {
  return new Promise(resolve => {
    if (this.sid() === DEFAULT_SESSION_ID) {
      resolve(false);
      return;
    }

    if (uci.applying()) {
      resolve(true);
      return;
    }

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

session.get = function(cb) {
  ubus.call('session', 'get').then(r => {
    cb(r.values);
  }).catch(() => {
    cb();
  });
}

session.startHeartbeat = function() {
  if (typeof(this._hearbeatInterval) !== 'undefined') {
    return;
  }

  this._hearbeatInterval = window.setInterval(() => {
    this.isAlive().then(alive => {
      if (!alive) {
        this.logout();
      }
    });
  }, 15000);
}

session.stopHeartbeat = function() {
  if (typeof(this._hearbeatInterval) !== 'undefined') {
    window.clearInterval(this._hearbeatInterval);
    delete this._hearbeatInterval;
  }
}

session.updateACLs = function() {
  return new Promise(resolve => {
    ubus.call('session', 'access').then(acls => {
      this.aclCache = acls || {};
      resolve();
    });
  });
}

session.hasACL = function(scope, object, func) {
  const acls = this.aclCache;

  if (typeof(func) === 'undefined')
    return (acls && acls[scope] && acls[scope][object]);

  if (acls && acls[scope] && acls[scope][object])
    for (let i = 0; i < acls[scope][object].length; i++)
      if (acls[scope][object][i] === func)
        return true;

  return false;
}

export default {
  install(Vue) {
    Vue.prototype.$session = session;
  }
}
