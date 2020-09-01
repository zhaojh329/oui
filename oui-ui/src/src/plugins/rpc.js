import axios from 'axios'

export const rpc = {}

let RpcID = 1

function sid () {
  return sessionStorage.getItem('sid') || ''
}

rpc.request = function (method, params, timeout) {
  if (typeof (timeout) !== 'number' || timeout < 1) { timeout = 10000 }

  return new Promise((resolve, reject) => {
    const req = {
      jsonrpc: '2.0',
      id: RpcID++,
      method,
      params
    }

    axios.post('/rpc', req, {
      timeout: timeout
    }).then(res => {
      res = res.data

      if (typeof (res) !== 'object' || res.jsonrpc !== '2.0') { throw new Error('rpc request: Invalid msg') }

      if (res.id !== req.id) { throw new Error('No related request for JSON response') }

      if (typeof (res.error) === 'object') {
        reject(JSON.stringify({ req, error: res.error }))
        return
      }

      resolve(res.result)
    }).catch(err => reject(err))
  })
}

rpc.call = function (object, method, params, timeout) {
  if (typeof params !== 'object') params = {}
  return this.request('call', [sid(), object, method, params], timeout)
}

rpc.ubus = function (object, method, params, timeout) {
  return this.request('call', [sid(), 'ubus', 'call', { object, method, params }], timeout)
}

rpc.exec = function (cmd, params, timeout) {
  return this.request('exec', [sid(), cmd, ...params], timeout)
}

rpc.login = function (password) {
  return this.request('login', {
    password
  })
}

rpc.logout = function () {
  return this.request('logout', { sid: sid() })
}

rpc.alive = function () {
  return this.request('alive', { sid: sid() })
}

export default {
  install (Vue) {
    Vue.prototype.$rpc = rpc
  }
}
