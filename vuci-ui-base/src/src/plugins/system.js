import * as ubus from './ubus.js'

function getInfo() {
  return new Promise(function(resolve) {
    let req = [{
      object: 'system',
      method: 'info'
    }, {
      object: 'system',
      method: 'board'
    }];
    ubus.callBatch(req).then((r) => {
      if (r)
        resolve(Object.assign({}, r[0], r[1]))
    });
  });
}

const system = {}

system.install = function(Vue) {
  Vue.prototype.$system = {
    getInfo: getInfo
  }
}

export default system
