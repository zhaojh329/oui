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
    /* Whether the name property is a uci option */
    uci: {
      type: Boolean,
      default: true
    },
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
    /*
    ** depend="x"
    ** depend="y == 'test'"
    ** depend="x, y == 'test'"
    */
    depend: {
      type: String,
      default: ''
    },
    placeholder: String,
    /* Used for multiple list */
    multiple: Boolean
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
    rules() {
      return this.uciForm.rules;
    },
    validates() {
      return this.uciForm.validates;
    },
    depends() {
      const compares = ['==', '>', '<', '>=', '<=', '!='];
      const depends = {};

      this.depend.split(',').forEach(d => {
        if (d === '')
          return;

        d = d.trim().replace(/\s+/g, ' ').split(' ');
        let [name, compare, value] = d;

        if (d.length === 1) {
          value = '1',
          compare = '==';
        } else if (d.length !== 3) {
          return;
        }

        if (compares.indexOf(compare) === -1)
          return;

        depends[name] = `${compare} ${value}`;
      });

      return depends;
    },
    transformedOptions() {
      return this.options.map(o => {
        if (typeof(o) === 'string')
          return [o, o];
        if (o.length === 1)
          return [o[0], o[0]];
        return o;
      });
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
      this.uciSection.toggle(this.name);
    },
    buildForm(sid) {
      const prop = this.prop(sid);

      this.$set(this.validates, prop, {valid: true, tab: this.tab});

      const rule = [];
      if (this.required)
        rule.push({required: true, message: this.name + ' is required'});
      if (rule.length > 0)
        this.$set(this.rules, prop, rule);

      let value = undefined;

      if (this.load) {
        new Promise(resolve => {
          this.load(resolve);
        }).then(v => {
          this.buildFormValue(sid, v);
        });
      } else if (this.uci) {
        value = this.$uci.get(this.config, sid, this.name);
      }

      this.buildFormValue(sid, value);

      this.$watch(`form.${prop}`, () => {
        this.uciSection.toggle(this.name);
      });
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

      this.$uci.set(this.config, sid, this.name, value);
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
