const validator = {}

validator.types = {
  number: {
    validator: 'number',
    message: 'Must be a number'
  },
  integer: {
    validator: 'integer',
    message: 'Must be an integer'
  },
  float: {
    validator: 'float',
    message: 'Must be a float'
  },
  url: {
    validator: 'url',
    message: 'Must be a valid ur'
  },
  hex: {
    validator: 'hex',
    message: 'Must be a valid hex'
  },
  email: {
    validator: 'email',
    message: 'Must be a valid email'
  },
  hostname: {
    validator: (rule, value, callback) => {
      if ((value.length <= 253) &&
        ((value.match(/^[a-zA-Z0-9]+$/) !== null ||
        (value.match(/^[a-zA-Z0-9_][a-zA-Z0-9_\-.]*[a-zA-Z0-9]$/) &&
        value.match(/[^0-9.]/)))))
        callback();
      else
        callback(new Error('Must be a valid hostname'));
    }
  }
}

validator.compileString = function(rule) {
  const types = this.types;

  if (Object.keys(types).indexOf(rule) < 0)
    return [];

  const type = types[rule];
  const r = {};

  if (typeof(type.validator) === 'string') {
    r.type = type.validator;
    r.message = type.message;
  } else {
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