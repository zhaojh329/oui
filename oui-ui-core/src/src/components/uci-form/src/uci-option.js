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
    initial: String,
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
    buildForm(sid) {
      const prop = this.prop(sid);

      if (this.type === 'dlist')
        this.$set(this.form, prop, []);
      else if (this.type === 'list')
        this.$set(this.form, prop, this.multiple ? [] : '');
      else if (this.type === 'switch')
        this.$set(this.form, prop, '0');
      else
        this.$set(this.form, prop, '');

      this.$set(this.validates, prop, {valid: true, tab: this.tab});

      this.$watch(`form.${prop}`, () => {
        this.uciSection.toggle(this.name);
      });

      const rule = [];
      if (this.required)
        rule.push({required: true, message: this.name + ' is required'});
      if (rule.length > 0)
        this.$set(this.rules, prop, rule);

      if (typeof(this.load) !== 'undefined') {
        new Promise(resolve => {
          this.load(resolve);
        }).then(v => {
          this.$set(this.form, prop, v);
          this.original = v;
        });
        return;
      }

      let val = this.$uci.get(this.config, sid, this.name);
      if (typeof(val) !== 'undefined') {
        if (this.type === 'list' && this.multiple)
          val = val.replace(/\s+/g, ' ').split(' ');
      }

      if (typeof(val) === 'undefined' && typeof(this.initial) !== 'undefined')
        val = this.initial;

      if (typeof(val) === 'undefined') {
        if (this.type === 'dlist')
          val = [];
        else if (this.type === 'list')
          val = this.multiple ? [] : '';
        else if (this.type === 'switch')
          val = '0';
        else
          val = '';
      }

      this.$set(this.form, prop, val);
      this.original = val;

      this.uciSection.toggle(this.name);
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
