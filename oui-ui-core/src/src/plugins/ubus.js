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

      if (resp.id !== req.id) {
        reject('No related request for JSON response');
        return;
      }

      if (typeof(resp.error) === 'object') {
        reject(resp.error);
        return;
      }

      let result = resp.result;

      if (!Array.isArray(result) || result.length < 1) {
        if (req.method === 'call') {
          reject('Illegal response format');
          return;
        }
        result = [];
      }

      if (req.method === 'call') {
        const errCode = result[0];
        if (errCode !== 0) {
          const errInfo = ubusErrorInfo[errCode] || '';
          reject(`ubus error: ${errInfo}`);
          return;
        }
        result = result[1];
      }

      resolve(result);
    }).catch(error => {
      reject(error);
    });
  });
}

ubus.call = function(object, method, params) {
  if (typeof(params) === 'undefined')
    params = {};

  const req = {
    jsonrpc: '2.0',
    id: this.id++,
    method: 'call',
    params:  [session.sid(), object, method, params]
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