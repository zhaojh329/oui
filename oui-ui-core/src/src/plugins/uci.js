'use strict'

import {ubus} from './ubus'

const uci = {
  state: {
    newidx: 0,
    changed: 0,
    values: {},
    changes: {},
    creates: {},
    deletes: {}
  }
}

uci.load = function(conf, force) {
  return new Promise(resolve => {
    if (!force && this.state.values[conf]) {
      resolve();
      return;
    }

    ubus.call('uci', 'get', {config: conf}).then(r => {
      this.state.values[conf] = r.values;
      resolve();
    });
  });
}

uci.sections = function(conf, type) {
  const v = this.state.values[conf] || {};
  const n = this.state.creates[conf] || {};
  const d = this.state.deletes[conf] || {};
  const sections = [];

  sections.push(...Object.keys(v).map(sid => v[sid]).filter(s => {
    const sid = s['.name'];
    if (d && d[sid])
      return false;
    return !type || s['.type'] === type;
  }));
  sections.push(...Object.keys(n).map(sid => n[sid]).filter(s => !type || s['.type'] === type));

  return sections;
}

uci.get = function(conf, sid, opt) {
  const v = this.state.values[conf];
  const n = this.state.creates[conf];
  let s = undefined;

  if (typeof(sid) === 'undefined')
    return undefined;

  if (n && n[sid])
    s = n[sid];
  else if (v && v[sid])
    s = v[sid];

  if (typeof(opt) === 'undefined')
    return s;

  return s && s[opt];
}

uci.set = function(conf, sid, opt, val) {
  const v = this.state.values;
  const c = this.state.changes;
  const n = this.state.creates;

  if (typeof(sid) === 'undefined' ||
    typeof(opt) === 'undefined' ||
    opt.charAt(0) === '.')
    return;

  if (n[conf] && n[conf][sid]) {
    n[conf][sid][opt] = val;
    this.state.changed++;
    return;
  }

  if (v[conf] && v[sid]) {
    /* Ignore the same value */
    const old = v[conf][sid][opt];
    if (typeof(old) === 'undefined' && (val === '' || val === []))
      return;
    if (old === val)
      return;

    if (!c[conf])
      c[conf] = {};

    if (!c[conf][sid])
      c[conf][sid] = {};

    c[conf][sid][opt] = val;
    this.state.changed++;
  }
}

uci.createSID = function(conf) {
  const v = this.state.values;
  const n = this.state.creates;
  let sid;

  do {
    sid = 'new' + parseInt((Math.random() * 0xFFFFFF).toFixed(0)).toString(16);
  } while ((n[conf] && n[conf][sid]) || (v[conf] && v[conf][sid]));

  return sid;
},

uci.add = function(conf, type, name) {
  const n = this.state.creates;
  const sid = name || this.createSID(conf);

  if (!n[conf])
    n[conf] = {};

  n[conf][sid] = {
    '.type': type,
    '.name': sid,
    '.create': name,
    '.anonymous': !name,
    '.index': 1000 + this.state.newidx++
  };

  this.state.changed++;
  return sid;
}

uci.del = function(conf, sid) {
  const n = this.state.creates;
  const c = this.state.changes;
  const d = this.state.deletes;

  if (n[conf] && n[conf][sid]) {
    delete n[conf][sid];
  } else {
    if (c[conf])
      delete c[conf][sid];

    if (!d[conf])
      d[conf] = {};

    d[conf][sid] = true;
  }
  this.state.changed++;
}

uci.changed = function() {
  return this.state.changed;
}

uci.reset = function() {
  this.state.changes = {};
  this.state.creates = {};
  this.state.deletes = {};
  this.state.changed = 0;
  this.state.newidx = 0;
}

uci.save = function() {
  const c = this.state.changes;
  const n = this.state.creates;
  const d = this.state.deletes;

  return new Promise(resolve => {
    const batch = [];

    for (const conf in c) {
      for (const sid in c[conf]) {
        batch.push(['uci', 'set', {config: conf, section: sid, values: c[conf][sid]}]);
      }
    }

    for (const conf in n) {
      for (const sid in n[conf]) {
        const params = {config: conf, values: {}};
        for (const k in n[conf][sid]) {
          if (k === '.type')
            params.type = n[conf][sid][k];
          else if (k === '.create')
            params.name = n[conf][sid][k];
          else if (k.charAt(0) !== '.')
            params.values[k] = n[conf][sid][k];
        }
        batch.push(['uci', 'add', params]);
      }
    }

    for (const conf in d) {
      for (const sid in d[conf])
        batch.push(['uci', 'delete', {config: conf, section: sid}]);
    }

    this.reset();

    if (batch.length === 0) {
      resolve();
      return;
    }

    ubus.callBatch(batch).then(() => {
      resolve();
    });
  });
}

uci.apply = function(timeout) {
  const date = new Date();

  if (typeof(timeout) !== 'number' || timeout < 1)
    timeout = 10;

  return new Promise((resolve, reject) => {
    ubus.call('uci', 'apply', {rollback: true, timeout: timeout}).then(() => {
      const try_deadline = date.getTime() + 1000 * timeout;
      const try_confirm = function() {
        ubus.call('uci', 'confirm').then(() => {
          resolve();
        }).catch(e => {
          if (date.getTime() < try_deadline)
            window.setTimeout(try_confirm, 250);
          else
            reject(e);
        });
      };

      window.setTimeout(try_confirm, 1000);
    }).catch(e => {
      reject(e);
    });
  });
}

export default {
  install(Vue) {
    Vue.prototype.$uci = uci;
  }
}
