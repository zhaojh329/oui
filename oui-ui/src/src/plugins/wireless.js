import { rpc } from './rpc'

const wireless = {}

wireless.getDevices = function () {
  return new Promise(resolve => {
    if (this.devices) {
      resolve(this.devices)
      return
    }

    rpc.ubus('iwinfo', 'devices').then(r => {
      this.devices = r.devices || []
      resolve(this.devices)
    })
  })
}

wireless.getAssoclist = function () {
  return new Promise(resolve => {
    this.getDevices().then(devices => {
      const promises = []

      if (devices.length < 1) {
        resolve([])
        return
      }

      devices.forEach(dev => {
        promises.push(rpc.ubus('iwinfo', 'assoclist', { device: dev }))
      })

      Promise.all(promises).then(rs => {
        const assoclist = []

        rs.forEach(r => {
          assoclist.push(...r.results)
        })

        resolve(assoclist)
      })
    })
  })
}

export default {
  install (Vue) {
    Vue.prototype.$wireless = wireless
  }
}
