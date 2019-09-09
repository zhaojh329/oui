import validator from './validator'

export default {
  name: 'UciOption',
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
    ** Parameters: sid, self
    ** Return:  Promise object or value
    */
    load: [String, Array, Function],
    /*
    ** If a function provided, it will be called when oui saves the uci configuration.
    ** If an empty string provided, indicates don't save uci.
    ** If any other value provided, indicates save uci with the value.
    ** Parameters: sid, value, self
    ** Return:  Promise object or undefined
    */
    save: [String, Array, Function],
    /*
    ** If this function is provided, it will be called when oui applys the uci configuration.
    ** Parameters: value, self
    ** Return:  Promise object or undefined
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
    header: String,
    /* Used for table section */
    width: [String, Number],
    hide: Boolean
  },
  data() {
    return {
      /* original value */
      original: {},
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
    },
    isTableExpand() {
      let parent = this.$getParent('UciTableExpand');
      if (parent)
        return true;
      return false;
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
    setFormValue(sid, val) {
      this.form[this.formProp(sid)] = val;
    },
    textValue(sid) {
      let v = this.formValue(sid);

      if (typeof(v) === 'object') {
        if (Array.isArray(v))
          v = v.join(', ');
        else
          v = JSON.stringify(v);
      }

      return v;
    },
    dependExprValue(sid) {
      const v = this.formValue(sid);
      return `'${v}'`;
    },
    visible(sid) {
      const depend = this.parsedDepend;
      if (!depend)
        return true;

      let expr = depend.expr;

      depend.names.forEach(name => {
        const o = this.uciSection.children[name];
        if (!o)
          return false;
        let v = o.dependExprValue(sid);
        expr = expr.replace(new RegExp(name, 'gm'), v);
      });

      return eval(expr);
    },
    buildFormRule() {
      const rules = [];

      if (this.required)
        rules.push({required: true, message: this.$t('This field is required')});

      this.parsedRules.forEach(rule => {
        rule = validator.compile(rule, this);
        rules.push(...rule);
      });

      return rules;
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
        this.original[sid] = JSON.parse(JSON.stringify(value));
      else
        this.original[sid] = value;

      this.setFormValue(sid, value);

      this.$watch(`form.${prop}`, value => {
        this.$emit('change', value, sid, this);
      });

      this.$emit('change', value, sid, this);
    },
    buildFormSid(sid) {
      const prop = this.formProp(sid);
      const rules = this.buildFormRule(sid);
      const value = this.convertFromUCI();

      this.uciForm.addProp(prop, {value, rules, tab: this.tabName});

      if (typeof(this.load) === 'function') {
        const r = this.load(sid, this);
        if (window.oui.isPromise(r)) {
          r.then(value => {
            this.buildFormValue(sid, value);
          });
        } else {
          this.buildFormValue(sid, r);
        }
      } else if (typeof(this.load) !== 'undefined') {
        this.buildFormValue(sid, this.load);
        this.$watch('load', value => {
          this.setFormValue(sid, value);
        });
      } else {
        const value = this.getUciValue(sid);
        this.buildFormValue(sid, value);
      }
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
      this.uciForm.delProp(prop);
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
      if (this.save === '')
        return;

      let value = this.formValue(sid);
      if (typeof(this.save) !== 'undefined' && typeof(this.save) !== 'function')
        value = this.save;

      if (!this.visible(sid))
        value = this.convertFromUCI(this.initial);

      if (window.oui.isEqual(value, this.original[sid]))
        return;

      if (typeof(this.save) === 'function')
        return this.save(sid, value, this);

      this.$uci.set(this.config, sid, this.uciopt, this.convertToUCI(value));
    },
    _apply(sid) {
      const value = this.formValue(sid);

      if (window.oui.isEqual(value, this.original[sid]))
        return;

      if (typeof(this.apply) !== 'undefined')
        return this.apply(value, this);
    },
    applied(sid) {
      this.$emit('applied', this.formValue(sid));
    },
    view(prop, sid) {
      if (this.$scopedSlots.default)
        return this.$scopedSlots.default({sid, prop, value: this.formValue(sid), self: this});
      return '';
    }
  },
  created() {
    this.uid = this.uciForm.getUID();
    /* Ensure that option with the same name are deleted */
    this.$nextTick(() => {
      this.uciSection.addChild(this);
      if (this.uciSection.loaded)
        this.buildForm();
    });
  },
  render(h) {
    return h('div', this.$slots.default);
  },
  destroyed() {
    this.uciSection.delChild(this);
    this.destroyForm();
  }
}
