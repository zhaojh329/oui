import {session} from './session'
import {ubus} from './ubus'

export const uci = {
  state: {
    applying: false,
    newidx: 0,
    changed: 0,
    values: {},
    changes: {},
    creates: {},
    deletes: {},
    reorder: {}
  }
}

const isEmptyArray = window.oui.isEmptyArray

uci.load = function(conf) {
  return new Promise(resolve => {
    if (this.state.values[conf]) {
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

  return sections.sort((a, b) => a['.index'] - b['.index']);
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
  const d = this.state.deletes;

  if (typeof(sid) === 'undefined' ||
    typeof(opt) === 'undefined' ||
    opt.charAt(0) === '.')
    return;

  if (n[conf] && n[conf][sid]) {
    if (typeof(val) !== 'undefined' && val !== '' && !isEmptyArray(val)) {
      n[conf][sid][opt] = val;
    } else {
      delete n[conf][sid][opt];
    }
  } else if (typeof(val) !== 'undefined' && val !== '' && !isEmptyArray(val)) {
    /* do not set within deleted section */
    if (d[conf] && d[conf][sid] === true)
      return;

    /* only set in existing sections */
    if (!v[conf] || !v[conf][sid])
      return;

    if (!c[conf])
      c[conf] = {};

    if (!c[conf][sid])
      c[conf][sid] = {};

    /* undelete option */
    if (d[conf] && d[conf][sid]) {
      if (Array.isArray(d[conf][sid]))
        d[conf][sid].splice(d[conf][sid].indexOf(opt), 1);
    }

    c[conf][sid][opt] = val;
    this.state.changed++;
  } else {
    /* only delete in existing sections */
    if (!v[conf] || !v[conf][sid])
      return;

    if (!d[conf])
      d[conf] = {};

    if (!d[conf][sid])
      d[conf][sid] = [];

    if (d[conf][sid] !== true)
      d[conf][sid].push(opt);

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
}

uci.swap = function(conf, sid1, sid2) {
  const s1 = this.get(conf, sid1);
  const s2 = this.get(conf, sid2);
  const n1 = s1 ? s1['.index'] : NaN;
  const n2 = s2 ? s2['.index'] : NaN;

  if (isNaN(n1) || isNaN(n2))
    return false;

  s1['.index'] = n2;
  s2['.index'] = n1;

  this.state.reorder[conf] = true;
  this.state.changed++;

  return true;
},

uci.changed = function() {
  let changed = this.state.changed;
  const n = this.state.creates;
  const d = this.state.deletes;

  for (const conf in n)
    changed += Object.keys(n[conf]).length;

  for (const conf in d)
    changed += Object.keys(d[conf]).length;

  return changed;
}

uci.reset = function() {
  this.state.changes = {};
  this.state.creates = {};
  this.state.deletes = {};
  this.state.reorder = {};
  this.state.changed = 0;
  this.state.newidx = 0;
}

uci.reorderSections = function() {
  const v = this.state.values;
  const n = this.state.creates;
  const r = this.state.reorder;

  const batch = [];

  /*
  ** gather all created and existing sections, sort them according
  ** to their index value and issue an uci order call
  */
  for (const conf in r) {
    const orders = [];

    if (n[conf]) {
      for (const s in n[conf])
        orders.push(n[conf][s]);
    }

    for (const s in v[conf])
      orders.push(v[conf][s]);

    orders.sort((a, b) => a['.index'] - b['.index']);
    const sids = orders.map(o => o['.name']);

    if (sids.length > 0)
      batch.push(['uci', 'order', {config: conf, sections: sids}]);
  }

  return new Promise(resolve => {
    if (batch.length === 0) {
      resolve();
      return;
    }

    ubus.callBatch(batch).then(() => {
      resolve();
    });
  });
}

uci.save = function() {
  const c = this.state.changes;
  const n = this.state.creates;
  const d = this.state.deletes;

  return new Promise(resolve => {
    const batch = [];
    const confs = {};

    for (const conf in c) {
      for (const sid in c[conf])
        batch.push(['uci', 'set', {config: conf, section: sid, values: c[conf][sid]}]);
      confs[conf] = true;
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
      confs[conf] = true;
    }

    for (const conf in d) {
      for (const sid in d[conf]) {
        const options = d[conf][sid];
        batch.push(['uci', 'delete', {config: conf, section: sid, options: (options === true) ? undefined : options}]);
      }
      confs[conf] = true;
    }

    if (batch.length === 0) {
      this.reorderSections().then(() => {
        this.reset();
        for (const conf in confs)
          delete this.state.values[conf];
        resolve();
      });
      return;
    }

    ubus.callBatch(batch).then(() => {
      this.reorderSections().then(() => {
        this.reset();
        for (const conf in confs)
          delete this.state.values[conf];
        resolve();
      });
    });
  });
}

uci.apply = function(timeout) {
  const state = this.state;
  const date = new Date();

  if (typeof(timeout) !== 'number' || timeout < 1)
    timeout = 10;

  state.applying = true;

  return new Promise((resolve, reject) => {
    ubus.call('uci', 'apply', {rollback: true, timeout: timeout}).then(() => {
      const try_deadline = date.getTime() + 1000 * timeout;
      const try_confirm = function() {
        ubus.call('uci', 'confirm').then(() => {
          state.applying = false;
          resolve();
        }).catch(e => {
          if (date.getTime() < try_deadline) {
            window.setTimeout(try_confirm, 250);
            return;
          }
          state.applying = false;
          reject(e);
        });
      };

      window.setTimeout(try_confirm, 1000);
    }).catch(e => {
      state.applying = false;
      reject(e);
    });
  });
}

uci.applying = function() {
  return this.state.applying;
}

uci.readable = function(conf) {
  return session.hasACL('uci', conf, 'read');
}

uci.writable = function(conf) {
  return session.hasACL('uci', conf, 'write');
}

export default {
  install(Vue) {
    Vue.prototype.$uci = uci;
  }
}
