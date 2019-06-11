const validator = {}

function performCallback(types, rule, value, cb, msg, arg) {
  if (value === '' || types[rule.type].verify(value, arg))
    cb();
  else
    cb(new Error(msg));
}

const types = {
  url: {
    validator: 'url',
    message: 'Must be a valid ur'
  },
  email: {
    validator: 'email',
    message: 'Must be a valid email'
  },
  number: {
    verify: (value) => {
      return !isNaN(value);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, 'Must be a number');
    }
  },
  integer: {
    verify: (value) => {
      return types['number'].verify(value) && parseInt(value).toString() === value;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, 'Must be a integer');
    }
  },
  uinteger: {
    verify: (value) => {
      return types['integer'].verify(value) && value >= 0;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, 'Must be a positive integer');
    }
  },
  float: {
    verify: (value) => {
      return types['number'].verify(value) && parseFloat(value).toString() === value;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, 'Must be a number');
    }
  },
  ufloat: {
    verify: (value) => {
      return types['float'].verify(value) && value >= 0;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, 'Must be a positive number');
    }
  },
  min: {
    verify: (value, arg) => {
      return types['float'].verify(value) && value >= arg;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, `Must be a number greater or equal to ${rule.arg}`, rule.arg);
    }
  },
  max: {
    verify: (value, arg) => {
      return types['float'].verify(value) && value <= arg;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, `Must be a number lower or equal to ${rule.arg}`, rule.arg);
    }
  },
  range: {
    verify: (value, arg) => {
      return types['min'].verify(value, arg[0]) && types['max'].verify(value, arg[1]);
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, `Must be a number between ${rule.arg[0]} and ${rule.arg[1]}`, rule.arg);
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
      performCallback(types, rule, value, cb, 'Must be a valid hostname');
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
    r.message = type.message;
  } else {
    r.type = rule;
    r.arg = arg;
    r.validator = type.validator;
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

validator.compile = function(rule) {
  const type = typeof(rule);
  if (type === 'string')
    return this.compileString(rule);

  if (type === 'object')
    return this.compileObject(rule);

  return [];
}

export default validator
