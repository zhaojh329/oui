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

ubus._call = function(reqs, timeout) {
  if (typeof(timeout) !== 'number' || timeout < 1)
    timeout = 10;

  return new Promise((resolve, reject) => {
    axios.post('/ubus', reqs, {
      timeout: timeout * 1000
    }).then(response => {
      let resp = response.data;

      if (!Array.isArray(reqs)) {
        reqs = [reqs];
        resp = [resp];
      }

      let data = [];

      for (let i = 0; i < resp.length; i++) {
        const req = reqs[i];

        if (typeof(resp[i]) !== 'object' || resp[i].jsonrpc !== '2.0')
          throw 'ubus call error: Invalid msg';

        if (resp[i].id !== reqs[i].id)
          throw 'No related request for JSON response';

        if (typeof(resp[i].error) === 'object') {
          reject({req: req.params.slice(1), error: resp[i].error});
          return;
        }

        let result = resp[i].result;

        if (!Array.isArray(result) || result.length < 1) {
          if (reqs[i].method === 'call') {
            throw 'Illegal response format';
          }
          result = [];
        }

        if (reqs[i].method === 'call') {
          const code = result[0];
          if (code !== 0) {
            const message = ubusErrorInfo[code] || '';
            reject({req: req.params.slice(1), error: {code, message}});
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

ubus.call = function(object, method, params, timeout) {
  const req = this._buildRequest(object, method, params);
  return this._call(req, timeout);
}

ubus.callBatch = function(batchs, timeout) {
  const reqs = [];

  batchs.forEach(item => {
    const req = this._buildRequest(item[0], item[1], item[2]);
    reqs.push(req);
  });

  return this._call(reqs, timeout);
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
