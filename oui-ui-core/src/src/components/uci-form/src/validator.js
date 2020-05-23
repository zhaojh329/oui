import i18n from '@/i18n'

const validator = {}

function parseIPv4(str) {
  if ((typeof(str) !== 'string' && !(str instanceof String)) ||
    !str.match(/^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/))
    return undefined;

  const num = [ ];
  const parts = str.split(/\./);

  for (let i = 0; i < parts.length; i++) {
    const n = parseInt(parts[i]);
    if (isNaN(n) || n > 255)
      return undefined;

    num.push(n);
  }

  return num;
}

function parseIPv6(str) {
  if ((typeof(str) !== 'string' && !(str instanceof String)) ||
    !str.match(/^[a-fA-F0-9:]+(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})?$/))
    return undefined;

  const parts = str.split(/::/);
  if (parts.length === 0 || parts.length > 2)
    return undefined;

  const lnum = [ ];
  if (parts[0].length > 0) {
    const left = parts[0].split(/:/);
    for (let i = 0; i < left.length; i++) {
      const n = parseInt(left[i], 16);
      if (isNaN(n))
        return undefined;

      lnum.push((n / 256) >> 0);
      lnum.push(n % 256);
    }
  }

  const rnum = [ ];
  if (parts.length > 1 && parts[1].length > 0) {
    const right = parts[1].split(/:/);

    for (let i = 0; i < right.length; i++) {
      if (right[i].indexOf('.') > 0) {
        const addr = parseIPv4(right[i]);
        if (!addr)
          return undefined;

        rnum.push.apply(rnum, addr);
        continue;
      }

      const n = parseInt(right[i], 16);
      if (isNaN(n))
        return undefined;

      rnum.push((n / 256) >> 0);
      rnum.push(n % 256);
    }
  }

  if (rnum.length > 0 && (lnum.length + rnum.length) > 15)
    return undefined;

  const num = [ ];

  num.push.apply(num, lnum);

  for (let i = 0; i < (16 - lnum.length - rnum.length); i++)
    num.push(0);

  num.push.apply(num, rnum);

  if (num.length > 16)
    return undefined;

  return num;
}

function isNetmask(addr) {
  if (!Array.isArray(addr))
    return false;

  let c;

  for (c = 0; (c < addr.length) && (addr[c] === 255); c++);

  if (c === addr.length)
    return true;

  if ((addr[c] === 254) || (addr[c] === 252) || (addr[c] === 248) ||
    (addr[c] === 240) || (addr[c] === 224) || (addr[c] === 192) ||
    (addr[c] === 128) || (addr[c] === 0)) {
    for (c++; (c < addr.length) && (addr[c] === 0); c++);

    if (c === addr.length)
      return true;
  }

  return false;
}

function performCallback(types, rule, value, cb, msg, arg) {
  if (value === undefined || value === '' || window.oui.isEmptyArray(value)) {
    cb();
    return;
  }

  if (Array.isArray(value)) {
    for (let i = 0; i < value.length; i++) {
      if (value[i] === '')
        continue;

      if (!types[rule.type].verify(value[i], arg)) {
        cb(new Error(msg));
        return;
      }
    }

    cb();
    return;
  }

  if (types[rule.type].verify(value, arg))
    cb();
  else
    cb(new Error(msg));
}

const types = {
  url: {
    validator: 'url',
    message: () => i18n.t('Must be a valid url')
  },
  email: {
    validator: 'email',
    message: () => i18n.t('Must be a valid email')
  },
  number: {
    verify: (value) => {
      return !isNaN(value);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a number'));
    }
  },
  integer: {
    verify: (value) => {
      return types['number'].verify(value) && parseInt(value).toString() === value;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a integer'));
    }
  },
  uinteger: {
    verify: (value) => {
      return types['integer'].verify(value) && value >= 0;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a positive integer'));
    }
  },
  min: {
    verify: (value, arg) => {
      return types['number'].verify(value) && value >= arg;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a number greater or equal to', {num: rule.arg}), rule.arg);
    }
  },
  max: {
    verify: (value, arg) => {
      return types['number'].verify(value) && value <= arg;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a number lower or equal to', {num: rule.arg}), rule.arg);
    }
  },
  range: {
    verify: (value, arg) => {
      return types['min'].verify(value, arg[0]) && types['max'].verify(value, arg[1]);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a number between', {min: rule.arg[0], max: rule.arg[1]}), rule.arg);
    }
  },
  hostname: {
    verify: (value) => {
      return (value.length <= 253) &&
        ((value.match(/^[a-zA-Z0-9]+$/) !== null ||
        (value.match(/^[a-zA-Z0-9_][a-zA-Z0-9_\-.]*[a-zA-Z0-9]$/) &&
        value.match(/[^0-9.]/))));
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid hostname'));
    }
  },
  ip4addr: {
    verify: (value) => {
      return parseIPv4(value);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid IPv4 address'));
    }
  },
  ip6addr: {
    verify: (value) => {
      return parseIPv6(value);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid IPv6 address'));
    }
  },
  ipaddr: {
    verify: (value) => {
      return types['ip4addr'].verify(value) || types['ip6addr'].verify(value);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid IP address'));
    }
  },
  netmask4: {
    verify: (value) => {
      return isNetmask(parseIPv4(value));
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid IPv4 netmask'));
    }
  },
  netmask6: {
    verify: (value) => {
      return isNetmask(parseIPv6(value));
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid IPv6 netmask'));
    }
  },
  host: {
    verify: (value) => {
      return types['hostname'].verify(value) || types['ipaddr'].verify(value);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid hostname or IP address'));
    }
  },
  port: {
    verify: (value) => {
      return types['uinteger'].verify(value) && value <= 65535;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid port number'));
    }
  },
  macaddr: {
    verify: (value) => {
      return value.match(/^([a-fA-F0-9]{2}:){5}[a-fA-F0-9]{2}$/) !== null;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid MAC address'));
    }
  },
  uciname: {
    verify: (value) => {
      return value.match(/^[a-zA-Z0-9_]+$/) !== null;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, i18n.t('Must be a valid UCI identifier'));
    }
  }
}

validator.compileString = function(rule, arg) {
  if (Object.keys(types).indexOf(rule) < 0)
    return [];

  const type = types[rule];
  const r = {};

  if (typeof(type.validator) === 'string') {
    r.type = type.validator;

    if (typeof(type.message) === 'string')
      r.message = type.message;
    else if (typeof(type.message) === 'function')
      r.message = type.message();
  } else {
    r.type = rule;
    r.arg = arg;
    r.validator = type.validator;
  }

  return [r];
}

validator.compileFunction = function(verify, vm) {
  const r = {
    validator: (rule, value, callback) => {
      const err = verify(value, vm);
      if (typeof(err) === 'string')
        callback(new Error(err));
      else
        callback();
    }
  }

  return [r];
}

validator.compileObject = function(rule) {
  const rs = [];

  if (rule.type) {
    const r = this.compileString(rule.type);
    rs.push(...r);
  }

  if (typeof(rule.min) !== 'undefined' && typeof(rule.max) !== 'undefined') {
    const r = this.compileString('range', [rule.min, rule.max]);
    rs.push(...r);
  } else {
    if (typeof(rule.min) !== 'undefined') {
      const r = this.compileString('min', rule.min);
      rs.push(...r);
    }

    if (typeof(rule.max) !== 'undefined') {
      const r = this.compileString('max', rule.max);
      rs.push(...r);
    }
  }

  return rs;
}

/* vm: uci-option instance */
validator.compile = function(rule, vm) {
  const type = typeof(rule);
  if (type === 'string')
    return this.compileString(rule);

  if (type === 'function')
    return this.compileFunction(rule, vm);

  if (type === 'object')
    return this.compileObject(rule);

  return [];
}

export default validator
