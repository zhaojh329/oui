"use strict";

const axios = require('axios')

const ubus = {
  id: 1,
  sid: '00000000000000000000000000000000'
}

ubus._call = function(req) {
  return new Promise((resolve, reject) => {
    axios.post('/ubus', req, {
      responseType: 'text'
    }).then(response => {
      const resp = response.data;
      if (typeof(resp) !== 'object' || resp.jsonrpc !== '2.0') {
        reject('ubus call error: Invalid msg');
        return;
      }
      resolve(resp.result);
    }).catch(error => {
      reject(error);
    });
  });
}

ubus.call = function(object, method, params) {
  const req = {
    jsonrpc: '2.0',
    id: this.id++,
    method: 'call',
    params:  [this.sid, object, method, params]
  };

  return this._call(req);
}

ubus.list = function() {
  const params = Object.keys(arguments).map(k => arguments[k]);
  const req = {
    jsonrpc: '2.0',
    id: this.id++,
    method: 'list',
    params:  (params.length > 0) ? params : undefined
  };

  return this._call(req);
}

export default {
  install(Vue) {
    Vue.prototype.$ubus = ubus;
  }
}