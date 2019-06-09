'use strict'

import {ubus} from './ubus'

const uci = {
  state: {
    changed: 0,
    values: {},
    changes: {}
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
  const v = this.state.values[conf];

  if (!v)
    return [];

  return Object.keys(v).map(sid => v[sid]).filter(s => !type || s['.type'] === type);
}

uci.get = function(conf, sid, opt) {
  const v = this.state.values[conf];

  if (typeof(sid) === 'undefined')
    return undefined;

  if (!v || !sid)
    return v;

  const s = v[sid];

  if (!s || !opt)
    return s;

  return s[opt];
}

uci.set = function(conf, sid, opt, val) {
  const v = this.state.values;
  const c = this.state.changes;

  if (typeof(sid) === 'undefined' ||
    typeof(opt) === 'undefined' ||
    opt.charAt(0) === '.')
    return;

  if (!v[conf] || !v[conf][sid])
    return;

  /* Ignore the same value */
  const old = this.get(conf, sid, opt);
  if (typeof(old) === 'undefined' && (val === '' || val === []))
    return;
  if (old === val)
    return;

  if (!c[conf])
    c[conf] = {};

  if (!c[conf][sid])
    c[conf][sid] = {};

  c[conf][sid][opt] = val || '';
  this.state.changed++;
}

uci.changed = function() {
  return this.state.changed;
}

uci.reset = function() {
  this.state.changes = {};
}

uci.save = function() {
  const c = this.state.changes;

  this.state.changed = 0;

  return new Promise(resolve => {
    const batch = [];

    for (const conf in c) {
      for (const sid in c[conf]) {
        batch.push(['uci', 'set', {config: conf, section: sid, values: c[conf][sid]}]);
      }
      delete c[conf];
    }

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
