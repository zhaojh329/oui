import validator from './validator'

export default {
  name: 'UciOption',
  inject: ['uciForm', 'uciSection'],
  props: {
    type: {
      required: true,
      validator: value => {
        return ['input', 'dummy', 'list', 'dlist', 'switch'].indexOf(value) !== -1;
      }
    },
    label: String,
    name: {
      type: String,
      required: true
    },
    required: Boolean,
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: [String, Array],
    /* Whether to access the uci value */
    uci: {
      type: Boolean,
      default: true
    },
    /* If this prop is provided, the uci value will be accessed with this prop instead of the name prop. */
    uciOption: String,
    /* Used for switch */
    activeValue: {
      type: String,
      default: '1'
    },
    inactiveValue: {
      type: String,
      default: '0'
    },
    /* Used for list */
    options: {
      type: Array,
      default() {
        return [];
      }
    },
    /* If this function is provided, the form loads the value by the function instead of from uci */
    load: Function,
    /*
    ** If this function is provided, it will be called when oui saves the uci configuration.
    ** If it returns false, it will terminate the save inside oui.
    */
    save: Function,
    /* If this function is provided, it will be called when oui applys the uci configuration. */
    apply: Function,
    /* depend="a == 12 || a == 'x' && y == 4 && q != 5 && !z" */
    depend: {
      type: String,
      default: ''
    },
    placeholder: String,
    /* Used for multiple list */
    multiple: Boolean,
    /* validator rules */
    rules: [String, Object, Function, Array]
  },
  data() {
    return {
      /* original value */
      original: null
    }
  },
  computed: {
    tab() {
      const parent = this.$parent;
      if (parent.$options.name === 'UciTab')
        return parent.name;
      return undefined;
    },
    config() {
      return this.uciForm.config;
    },
    form() {
      return this.uciForm.form;
    },
    parsedDepend() {
      const compares = ['==', '===', '!=', '!==', '<', '>', '>=', '<='];
      const expr = this.depend.trim().replace(/\s+/g, ' ');
      const states = {
        name: 0,
        operand: 1,
        logic: 2,
        logicOrCmp: 3
      }

      if (expr === '')
        return undefined;

      const parts = expr.split(' ');

      let s = states.name;
      const names = {};

      for (let i = 0; i < parts.length; i++) {
        let part = parts[i];

        if (s === states.name) {
          if (part[0] === '!') {
            part = part.substr(1);
            s = states.logic;
          } else {
            s = states.logicOrCmp;
          }

          if (!/^[A-Za-z_]/.test(part))
            return undefined;
          names[part] = true;

          if (i === parts.length - 1)
            return {expr, names: Object.keys(names)};

          continue;
        }

        if (s === states.logicOrCmp) {
          if (i === parts.length - 1)
            return undefined;

          if (compares.indexOf(part) > -1) {
            s = states.operand;
            continue;
          }

          if (part === '||' || part === '&&') {
            s = states.name;
            i++;
            continue;
          }

          return undefined;
        }

        if (s === states.operand) {
          s = states.logic;

          const starts = part[0];
          const end = part[part.length - 1];

          if (starts === '\'' && end === '\'') {
            part = part.substr(1, part.length - 2);
            if (typeof(part) !== 'string')
              return undefined;
            continue;
          }

          if (starts !== '\'' && end !== '\'') {
            if (isNaN(part))
              return undefined;
            continue;
          }

          return undefined;
        }

        if (s === states.logic) {
          if (i === parts.length - 1)
            return undefined;

          if (part === '||' || part === '&&') {
            s = states.name;
            continue;
          }

          return undefined;
        }

        return undefined;
      }

      return {expr, names: Object.keys(names)};
    },
    transformedOptions() {
      return this.options.map(o => {
        if (typeof(o) === 'string')
          return [o, o];
        if (o.length === 1)
          return [o[0], o[0]];
        return o;
      });
    },
    uciOptName() {
      return this.uciOption || this.name;
    },
    parsedRules() {
      let rules = this.rules;

      if (typeof(rules) === 'string' || typeof(rules) === 'function')
        rules = [rules];

      if (typeof(rules) === 'object' && !Array.isArray(rules))
        rules = [rules];

      if (typeof(rules) === 'undefined')
        rules = [];

      return rules;
    }
  },
  created() {
    if (this.tab)
      this.$parent.options.push(this);
    else
      this.uciSection.options.push(this);
  },
  methods: {
    prop(sid) {
      return `${sid}_${this.name}`;
    },
    formValue(sid) {
      return this.form[this.prop(sid)];
    },
    buildFormRule(sid) {
      const rules = [];

      if (this.required)
        rules.push({required: true, message: this.$t('This field is required')});

      this.parsedRules.forEach(rule => {
        rule = validator.compile(rule);
        rules.push(...rule);
      });

      const prop = this.prop(sid);
      this.$set(this.uciForm.rules, prop, rules);
      this.$set(this.uciForm.validates, prop, {valid: true, tab: this.tab, sid: sid});
    },
    buildFormValue(sid, value) {
      if (typeof(value) === 'undefined' && typeof(this.initial) !== 'undefined')
        value = this.initial;

      if (typeof(value) !== 'undefined') {
        if (this.type === 'list' && this.multiple)
          value = value.replace(/\s+/g, ' ').split(' ');
      }

      if (typeof(value) === 'undefined') {
        if (this.type === 'dlist')
          value = [];
        else if (this.type === 'list')
          value = this.multiple ? [] : '';
        else if (this.type === 'switch')
          value = '0';
        else
          value = '';
      }

      this.original = value;
      this.$set(this.form, this.prop(sid), value);
    },
    buildForm(sid) {
      let value = undefined;

      if (this.load) {
        new Promise(resolve => {
          this.load(resolve);
        }).then(v => {
          this.buildFormValue(sid, v);
        });
      } else if (this.uci) {
        value = this.$uci.get(this.config, sid, this.uciOptName);
      }

      this.buildFormValue(sid, value);
      this.buildFormRule(sid);
    },
    saveUCI(sid) {
      if (this.type === 'dummy')
        return;

      let value = this.formValue(sid);
      if (value === this.original)
        return;

      if (typeof(this.save) !== 'undefined') {
        const ret = this.save(this.config, sid, this.name, value);
        if (ret === false)
          return;
      }

      if (!this.uci)
        return;

      if (this.type === 'list' && this.multiple)
        value = value.join(' ');

      this.$uci.set(this.config, sid, this.uciOptName, value);
    },
    applyUCI(sid) {
      if (this.type === 'dummy')
        return null;

      const value = this.formValue(sid);
      if (value === this.original)
        return null;

      if (typeof(this.apply) !== 'undefined') {
        const p = new Promise(resolve => {
          this.apply(resolve, value);
        });
        return p;
      }

      return null;
    }
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
