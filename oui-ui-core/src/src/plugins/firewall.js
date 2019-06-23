import {uci} from './uci'

const firewall = {
  zones: [],
  forwards: []
}

class Zone {
  constructor(sid) {
    this.sid = sid;
  }

  get(key) {
    return uci.get('firewall', this.sid, key);
  }

  set(key, val) {
    return uci.set('firewall', this.sid, key, val);
  }

  name() {
    return this.get('name');
  }

  network() {
    let nets = this.get('network') || [];

    if (!Array.isArray)
      nets = [nets];

    return nets;
  }

  addNetwork(net) {
    let nets = this.get('network');

    if (nets.indexOf(net) < 0)
      nets.push(net);

    this.set('network', nets);
  }

  delNetwork(net) {
    let nets = this.get('network');

    if (nets.indexOf(net) < 0)
      return;

    nets.splice(nets.indexOf(net), 1);
    this.set('network', nets);
  }

  findForwardsBy(what) {
    const forwards = [];
    const name = this.name();

    if (what !== 'src' && what !== 'dest')
      return forwards;

    firewall.forwards.forEach(fwd => {
      if (fwd.src() && fwd.dest() && fwd[what]() === name)
        forwards.push(fwd);
    });

    return forwards;
  }
}

class Forward {
  constructor(sid) {
    this.sid = sid;
  }

  get(key) {
    return uci.get('firewall', this.sid, key);
  }

  set(key, val) {
    return uci.set('firewall', this.sid, key, val);
  }

  src() {
    return this.get('src');
  }

  dest() {
    return this.get('dest');
  }
}

firewall.load = function() {
  return new Promise(resolve => {
    uci.load('firewall').then(() => {
      this.zones = uci.sections('firewall', 'zone').map(s => new Zone(s['.name']));
      this.forwards = uci.sections('firewall', 'forwarding').map(s => new Forward(s['.name']));
      resolve();
    });
  });
}

firewall.findZoneByNetwork = function(net) {
  for (let i = 0; i < this.zones.length; i++) {
    const zone = this.zones[i];
    const nets = zone.get('network');
    if (nets.indexOf(net) > -1)
      return zone;
  }
  return undefined;
}

firewall.findZoneByName = function(name) {
  for (let i = 0; i < this.zones.length; i++) {
    const zone = this.zones[i];
    if (zone.name() === name)
      return zone;
  }
  return undefined;
}

firewall.findZoneBySid = function(sid) {
  for (let i = 0; i < this.zones.length; i++) {
    const zone = this.zones[i];
    if (zone.sid === sid)
      return zone;
  }
  return undefined;
}

firewall.createZone = function(name) {
  const sid = uci.add('firewall', 'zone');
  const z = new Zone(sid);
  this.zones.push(z);

  z.set('name', name);
  z.set('input', 'ACCEPT');
  z.set('forward', 'REJECT');
  z.set('output', 'ACCEPT');

  return z;
}

export default {
  install(Vue) {
    Vue.prototype.$firewall = firewall;
  }
}
