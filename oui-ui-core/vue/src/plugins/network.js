import { rpc } from './rpc'

const network = {
  interfaces: [],
  devices: []
}

class Interface {
  constructor (iface) {
    this.status = iface
    this.name = iface.interface
  }

  isUp () {
    return this.status.up
  }

  getUptime () {
    const uptime = this.status.uptime
    return isNaN(uptime) ? 0 : uptime
  }

  getDevice () {
    const l3dev = this.status.l3_device
    if (l3dev) { return network.getDevice(l3dev) }

    return undefined
  }

  getAddrs (key, mask) {
    const rv = []
    const addrs = this.status[key]

    if (addrs) {
      addrs.forEach(addr => {
        let address = addr.address
        if (mask) { address += `/${addr.mask}` }
        rv.push(address)
      })
    }

    return rv
  }

  getIPv4Addrs (mask) {
    return this.getAddrs('ipv4-address', mask)
  }

  getIPv6Addrs (mask) {
    const rv = []

    rv.push(...this.getAddrs('ipv6-address', mask))
    rv.push(...this.getAddrs('ipv6-prefix-assignment', mask))

    return rv
  }

  getIPv4Gateway () {
    const rt = this.status.route

    if (rt) {
      for (let i = 0; i < rt.length; i++) {
        if (rt[i].target === '0.0.0.0' && rt[i].mask === 0) { return rt[i].nexthop }
      }
    }

    return undefined
  }

  getIPv6Gateway () {
    const rt = this.status.route

    if (rt) {
      for (let i = 0; i < rt.length; i++) {
        if (rt[i].target === '::' && rt[i].mask === 0) { return rt[i].nexthop }
      }
    }

    return undefined
  }

  getDNSAddrs () {
    const dns4 = this.getIPv4DNS()
    const dns6 = this.getIPv6DNS()

    return [...dns4, ...dns6]
  }

  getIPv4DNS () {
    const rv = []
    const dns = this.status['dns-server']

    if (dns) {
      for (let i = 0; i < dns.length; i++) {
        if (dns[i].indexOf(':') === -1) { rv.push(dns[i]) }
      }
    }

    return rv
  }

  getIPv6DNS () {
    const rv = []
    const dns = this.status['dns-server']

    if (dns) {
      for (let i = 0; i < dns.length; i++) {
        if (dns[i].indexOf(':') > -1) { rv.push(dns[i]) }
      }
    }

    return rv
  }

  getStatistics () {
    const dev = this.getDevice()
    if (dev) { return dev.statistics }
    return {}
  }
}

network.load = function () {
  const promises = []

  promises.push(rpc.ubus('network.device', 'status'))
  promises.push(rpc.ubus('network.interface', 'dump'))

  return new Promise(resolve => {
    Promise.all(promises).then(r => {
      const devices = r[0]

      this.devices = Object.keys(devices).map(name => {
        return { name: name, ...devices[name] }
      }).filter(d => d.name !== 'lo')

      this.interfaces = r[1].interface.map(iface => new Interface(iface)).filter(i => i.name !== 'loopback')

      resolve()
    })
  })
}

network.getInterface = function (name) {
  const interfaces = this.interfaces

  for (let i = 0; i < interfaces.length; i++) {
    if (interfaces[i].name === name) { return interfaces[i] }
  }

  return null
}

network.getInterfaces = function () {
  return this.interfaces
}

network.getDevice = function (name) {
  const devices = this.devices

  for (let i = 0; i < devices.length; i++) {
    if (devices[i].name === name) { return devices[i] }
  }

  return null
}

network.getDevices = function () {
  return this.devices
}

export default {
  install (Vue) {
    Vue.prototype.$network = network
  }
}
