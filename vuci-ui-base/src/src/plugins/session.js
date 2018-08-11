import * as ubus from './ubus.js'

const session = {}

function isAlive() {
  return new Promise(function(resolve) {
    ubus.call('session', 'access', {scope: 'ubus', object: 'session', function: 'access'}).then((r) => {
      resolve(r.access);
    }).catch(() => {
      resolve(false);
    });
  });
}

function stopHeartbeat() {
  if (typeof session._hearbeatInterval !== 'undefined') {
    window.clearInterval(session._hearbeatInterval);
    delete session._hearbeatInterval;
  }
}

function startHeartbeat() {
  stopHeartbeat();

  session._hearbeatInterval = window.setInterval(() => {
    isAlive().then((alive) => {
      if (!alive) {
        stopHeartbeat();
        sessionStorage.removeItem('_ubus_rpc_session');
        window.location.href = '/';
      }
    });
  }, 15000);
}

function login(username, password) {
  return new Promise(function(resolve, reject) {
    ubus.call('session', 'login', {username: username || '', password: password || ''}).then((r) => {
      if (r.ubus_rpc_session) {
        sessionStorage.setItem('_ubus_rpc_session', r.ubus_rpc_session);
        sessionStorage.setItem('username', r.data.username);
        startHeartbeat();
        resolve(r);
      }
    }).catch((r) => {
      reject(r);
    });
  });
}

session.install = function(Vue) {
  Vue.prototype.$session = {
    login: login,
    startHeartbeat: startHeartbeat
  }
}

export default session
