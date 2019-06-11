const validator = {}

function performCallback(types, rule, value, cb, msg) {
  if (value === '' || types[rule.type].verify(value))
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
  float: {
    verify: (value) => {
      return types['number'].verify(value) && parseFloat(value).toString() === value;
    },
    validator: (rule, value, cb) => {
      performCallback(types, rule, value, cb, 'Must be a float');
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

validator.compileString = function(rule) {
  if (Object.keys(types).indexOf(rule) < 0)
    return [];

  const type = types[rule];
  const r = {};

  if (typeof(type.validator) === 'string') {
    r.type = type.validator;
    r.message = type.message;
  } else {
    r.type = rule;
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

  if (rule.min) {
    return [{min: rule.min}];
  }

  if (rule.max) {
    return [{min: rule.max}];
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
