'use strict'

import axios from 'axios'
import {session} from './session'

export const ubus = {
  id: 1
}

const ubusErrorInfo = {
  0: 'Success',
  1: 'Invalid command',
  2: 'Invalid argument',
  3: 'Method not found',
  4: 'Not found',
  5: 'No response',
  6: 'Permission denied',
  7: 'Request timed out',
  8: 'Operation not supported',
  9: 'Unknown error',
  10: 'Connection failed'
}

ubus._call = function(reqs) {
  return new Promise((resolve, reject) => {
    axios.post('/ubus', reqs, {
      responseType: 'text'
    }).then(response => {
      let resp = response.data;

      if (!Array.isArray(reqs)) {
        reqs = [reqs];
        resp = [resp];
      }

      let data = [];

      for (let i = 0; i < resp.length; i++) {
        if (typeof(resp[i]) !== 'object' || resp[i].jsonrpc !== '2.0') {
          reject('ubus call error: Invalid msg');
          return;
        }

        if (resp[i].id !== reqs[i].id) {
          reject('No related request for JSON response');
          return;
        }

        if (typeof(resp[i].error) === 'object') {
          reject(resp[i].error);
          return;
        }

        let result = resp[i].result;

        if (!Array.isArray(result) || result.length < 1) {
          if (reqs[i].method === 'call') {
            reject('Illegal response format');
            return;
          }
          result = [];
        }

        if (reqs[i].method === 'call') {
          const errCode = result[0];
          if (errCode !== 0) {
            const errInfo = ubusErrorInfo[errCode] || '';
            reject(`ubus error: ${errInfo}`);
            return;
          }
          result = result[1];
        }

        data.push(result);
      }

      if (data.length === 0)
        data = undefined;
      else if (data.length === 1)
        data = data[0];

      resolve(data);
    }).catch(error => {
      reject(error);
    });
  });
}

ubus._buildRequest = function(object, method, params) {
  if (typeof(params) === 'undefined')
    params = {};

  const req = {
    jsonrpc: '2.0',
    id: this.id++,
    method: 'call',
    params:  [session.sid(), object, method, params]
  };

  return req;
}

ubus.call = function(object, method, params) {
  const req = this._buildRequest(object, method, params);
  return this._call(req);
}

ubus.callBatch = function(batchs) {
  const reqs = [];

  batchs.forEach(item => {
    const req = this._buildRequest(item[0], item[1], item[2]);
    reqs.push(req);
  });

  return this._call(reqs);
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