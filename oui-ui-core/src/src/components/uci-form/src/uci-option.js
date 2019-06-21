import validator from './validator'

export default {
  inject: ['uciForm', 'uciSection'],
  props: {
    label: String,
    name: {
      type: String,
      required: true
    },
    description: String,
    required: Boolean,
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: [String, Number],
    /* If this prop is provided, the uci value will be accessed with this prop instead of the name prop. */
    uciOption: String,
    /*
    ** If a function provided, the form loads the value by the function instead of from uci.
    ** If other type provided, the form loads the value from the prop's value.
    ** Parameters: resolve, sid, name, self
    */
    load: [String, Array, Function],
    /*
    ** If a function provided, it will be called when oui saves the uci configuration.
    ** If an any string provided, indicates don't save uci.
    ** Parameters: config, sid, name, value, self
    */
    save: [String, Function],
    /*
    ** If this function is provided, it will be called when oui applys the uci configuration.
    ** Parameters: resolve, value, self
    */
    apply: Function,
    /* depend="(a == 12 || a == 'x') && y == 4 && q != 5 && !z" */
    depend: {
      type: String,
      default: ''
    },
    /* validator rules */
    rules: [String, Object, Function, Array],
    tab: String,
    /* Used for custom header of table column */
    header: String
  },
  data() {
    return {
      /* original value */
      original: null,
      uid: -1
    }
  },
  computed: {
    tabName() {
      if (this.tab)
        return this.tab;
      let parent = this.$getParent('UciTab');
      if (parent)
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

      let waitRightBracket = 0;
      let s = states.name;
      const names = {};

      for (let i = 0; i < parts.length; i++) {
        let part = parts[i];

        if (s === states.name) {
          if (part[0] === '(') {
            waitRightBracket++;
            part = part.substr(1);
          }

          if (part[part.length - 1] === ')') {
            waitRightBracket--;
            part = part.substr(0, part.length - 1);
          }

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

          if (part[part.length - 1] === ')') {
            waitRightBracket--;
            part = part.substr(0, part.length - 1);
          }

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

      if (waitRightBracket !== 0)
        return undefined;

      return {expr, names: Object.keys(names)};
    },
    uciopt() {
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
  watch: {
    'uciSection.loaded'() {
      this.buildForm();
    }
  },
  methods: {
    formProp(sid) {
      return `${sid}_${this.uid}_${this.name}`;
    },
    formValue(sid) {
      return this.form[this.formProp(sid)];
    },
    dependExprValue(sid) {
      const v = this.formValue(sid);
      return `'${v}'`;
    },
    buildFormRule(sid) {
      const rules = [];

      if (this.required)
        rules.push({required: true, message: this.$t('This field is required')});

      this.parsedRules.forEach(rule => {
        rule = validator.compile(rule, this);
        rules.push(...rule);
      });

      const prop = this.formProp(sid);
      this.$set(this.uciForm.rules, prop, rules);
      this.$set(this.uciForm.validates, prop, {valid: true, tab: this.tabName, sid: sid});
    },
    convertFromUCI(value) {
      if (typeof(value) === 'undefined')
        value = '';
      return value.toString();
    },
    getUciValue(sid) {
      let value = this.$uci.get(this.config, sid, this.uciopt);
      if (typeof(value) === 'undefined')
        value = this.initial;
      return this.convertFromUCI(value);
    },
    buildFormValue(sid, value) {
      const prop = this.formProp(sid);

      if (typeof(value) === 'object')
        this.original = JSON.parse(JSON.stringify(value));
      else
        this.original = value;

      this.$set(this.form, prop, value);

      this.$watch(`form.${prop}`, value => {
        this.$emit('change', value, this);
      });

      this.$emit('change', value, this);
    },
    buildFormSid(sid) {
      let value = undefined;

      if (typeof(this.load) === 'function') {
        new Promise(resolve => {
          this.load(resolve, sid, this.name, this);
        }).then(v => {
          this.buildFormValue(sid, v);
        });
      } else if (typeof(this.load) !== 'undefined') {
        this.buildFormValue(sid, this.load);
        this.$watch('load', value => {
          this.$set(this.form, this.formProp(sid), value);
        });
      } else {
        value = this.getUciValue(sid);
      }

      this.buildFormValue(sid, value);
      this.buildFormRule(sid);
    },
    buildForm(sid) {
      if (sid) {
        this.buildFormSid(sid);
        return;
      }

      this.uciSection.sids.forEach(sid => {
        this.buildFormSid(sid);
      });
    },
    destroyFormSid(sid) {
      const prop = this.formProp(sid);
      this.$delete(this.uciForm.form, prop);
      this.$delete(this.uciForm.rules, prop);
      this.$delete(this.uciForm.validates, prop);
    },
    destroyForm() {
      this.uciSection.sids.forEach(sid => {
        this.destroyFormSid(sid);
      });
    },
    convertToUCI(value) {
      return value;
    },
    _save(sid) {
      let value = this.formValue(sid);

      if (window.oui.isEqual(value, this.original))
        return;

      if (this.save) {
        if (typeof(this.save) === 'function')
          this.save(this.config, sid, this.name, value, this);
        return;
      }

      this.$uci.set(this.config, sid, this.uciopt, this.convertToUCI(value));
    },
    _apply(sid) {
      const value = this.formValue(sid);

      if (window.oui.isEqual(value, this.original))
        return;

      if (typeof(this.apply) !== 'undefined') {
        const p = new Promise(resolve => {
          this.apply(resolve, value, this);
        });
        return p;
      }

      return null;
    }
  },
  created() {
    this.uid = this.uciForm.getUID();
    this.uciSection.addChild(this);

    if (this.uciSection.loaded)
      this.buildForm();
  },
  render(h) {
    return h('div', this.$slots.default);
  },
  destroyed() {
    this.uciSection.delChild(this);
    this.destroyForm();
  }
}
