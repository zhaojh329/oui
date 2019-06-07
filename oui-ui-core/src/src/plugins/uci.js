'use strict'

import {ubus} from './ubus'

const uci = {
  state: {
    changed: 0,
    values: {},
    changes: {}
  }
}

uci.load = function(config, force) {
  return new Promise(resolve => {
    if (!force && this.state.values[config]) {
      resolve();
      return;
    }

    ubus.call('uci', 'get', {config}).then(r => {
      this.state.values[config] = r.values;
      resolve();
    });
  });
}

uci.get = function(config, sid, opt) {
  const values = this.state.values[config];

  if (!values || !sid)
    return values;

  const s = values[sid];

  if (!s || !opt)
    return s;

  return s[opt];
}

uci.set = function(config, sid, opt, val) {
  const v = this.state.values;
  const c = this.state.changes;

  if (!v[config] || !v[config][sid])
    return;

  /* Ignore the same value */
  const old = this.get(config, sid, opt);
  if (typeof(old) === 'undefined' && (val === '' || val === []))
    return;
  if (old === val)
    return;

  if (!c[config])
    c[config] = {};

  if (!c[config][sid])
    c[config][sid] = {};

  c[config][sid][opt] = val || '';
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

    for (const config in c) {
      for (const sid in c[config]) {
        batch.push(['uci', 'set', {config: config, section: sid, values: c[config][sid]}]);
      }
      delete c[config];
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
