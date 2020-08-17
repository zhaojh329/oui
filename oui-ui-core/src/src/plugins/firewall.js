import { uci } from './uci'

const firewall = {
  zones: [],
  forwards: []
}

class Zone {
  constructor (sid) {
    this.sid = sid
  }

  get (key) {
    return uci.get('firewall', this.sid, key)
  }

  set (key, val) {
    return uci.set('firewall', this.sid, key, val)
  }

  name () {
    return this.get('name')
  }

  network () {
    let nets = this.get('network') || []

    if (!Array.isArray(nets)) { nets = nets.split(' ') }

    return nets
  }

  addNetwork (net) {
    const nets = this.network()

    if (nets.indexOf(net) > -1) { return }

    nets.push(net)
    this.set('network', nets.join(' '))
  }

  delNetwork (net) {
    const nets = this.network()

    if (nets.indexOf(net) < 0) { return }

    nets.splice(nets.indexOf(net), 1)
    this.set('network', nets.join(' '))
  }

  findForwardsBy (what) {
    const forwards = []
    const name = this.name()

    if (what !== 'src' && what !== 'dest') { return forwards }

    firewall.forwards.forEach(fwd => {
      if (fwd.src() && fwd.dest() && fwd[what]() === name) { forwards.push(fwd) }
    })

    return forwards
  }

  color () {
    const name = this.name()
    if (name === 'lan') { return '#90f090' } else if (name === 'wan') { return '#f09090' }

    const r = Math.floor(Math.random() * 128) + 1
    const g = Math.floor(Math.random() * 128) + 1
    let min = 0
    let max = 128

    if (r + g < 128) { min = 128 - r - g } else { max = 255 - r - g }

    const b = min + Math.floor(Math.random() * (max - min))

    return '#%02x%02x%02x'.format(0xFF - r, 0xFF - g, 0xFF - b)
  }
}

class Forward {
  constructor (sid) {
    this.sid = sid
  }

  get (key) {
    return uci.get('firewall', this.sid, key)
  }

  set (key, val) {
    return uci.set('firewall', this.sid, key, val)
  }

  src () {
    return this.get('src')
  }

  dest () {
    return this.get('dest')
  }
}

firewall.loadFromLocal = function () {
  this.zones = uci.sections('firewall', 'zone').map(s => new Zone(s['.name']))
  this.forwards = uci.sections('firewall', 'forwarding').map(s => new Forward(s['.name']))
}

firewall.load = function (local) {
  if (local) {
    this.loadFromLocal()
    return
  }

  return new Promise(resolve => {
    uci.load('firewall').then(() => {
      this.loadFromLocal()
      resolve()
    })
  })
}

firewall.findZoneByNetwork = function (net) {
  for (let i = 0; i < this.zones.length; i++) {
    const zone = this.zones[i]
    const nets = zone.network()
    if (nets.indexOf(net) > -1) { return zone }
  }
  return undefined
}

firewall.findZoneByName = function (name) {
  for (let i = 0; i < this.zones.length; i++) {
    const zone = this.zones[i]
    if (zone.name() === name) { return zone }
  }
  return undefined
}

firewall.findZoneBySid = function (sid) {
  for (let i = 0; i < this.zones.length; i++) {
    const zone = this.zones[i]
    if (zone.sid === sid) { return zone }
  }
  return undefined
}

firewall.createZone = function (name) {
  const sid = uci.add('firewall', 'zone')
  const z = new Zone(sid)
  this.zones.push(z)

  z.set('name', name)
  z.set('input', 'ACCEPT')
  z.set('forward', 'REJECT')
  z.set('output', 'ACCEPT')

  return z
}

export default {
  install (Vue) {
    Vue.prototype.$firewall = firewall
  }
}
