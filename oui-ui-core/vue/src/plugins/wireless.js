import { rpc } from './rpc'

const wireless = {}

wireless.getDevices = function () {
  return new Promise(resolve => {
    if (this.devices) {
      resolve(this.devices)
      return
    }

    rpc.call('iwinfo', 'devices').then(devices => {
      this.devices = devices
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
        promises.push(rpc.call('iwinfo', 'assoclist', { device: dev }))
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
