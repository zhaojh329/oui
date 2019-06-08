export default {
  name: 'UciSection',
  provide() {
    return {
      uciSection: this
    };
  },
  inject: ['uciForm'],
  props: {
    name: String,
    title: String,
    typed: Boolean
  },
  data() {
    return {
      formBuilt: false,
      tabs: [],
      options: [],
      values: {}
    }
  },
  computed: {
    loaded() {
      return this.uciForm.loaded;
    },
    uciSections() {
      const values = this.values;
      const matchKey = this.typed ? '.type' : '.name';
      const sections = [];

      Object.keys(values).forEach(k => {
        if (values[k][matchKey] === this.name)
          sections.push(values[k]);
      });
      return sections;
    }
  },
  watch: {
    loaded() {
      this.values = this.$uci.get(this.uciForm.config);
      this.$nextTick(() => {
        this.uciForm.buildForm();
      });
    }
  },
  created() {
    this.uciForm.sections.push(this);
  },
  methods: {
    findOptionByName(name) {
      for (let i = 0; i < this.options.length; i++)
        if (this.options[i].name === name)
          return this.options[i];

      for (let i = 0; i < this.tabs.length; i++) {
        const options = this.tabs[i].options;
        for (let j = 0; j < options.length; j++)
          if (options[j].name === name)
            return options[j];
      }
      return null;
    },
    toggleOptions(options, changedOptionName, sid) {
      options.forEach(o => {
        if (o.name === changedOptionName)
          return;

        const depends = o.depends;
        if (Object.keys(depends).indexOf(changedOptionName) > -1) {
          let visible = true;

          for (let name in depends) {
            const dependOpt = this.findOptionByName(name);
            if (!dependOpt)
              continue;

            const expr = `"${dependOpt.formValue(sid)}" ${depends[name]}`
            if (!eval(expr)) {
              visible = false;
              break;
            }
          }
          this.uciForm.$refs[o.prop(sid)][0].visible = visible;
        }
      });
    },
    toggle(name) {
      const sections = this.uciSections;

      sections.forEach(s => {
        const sid = s['.name'];
        this.toggleOptions(this.options, name, sid);
        this.tabs.forEach(tab => {
          this.toggleOptions(tab.options, name, sid);
        });
      });
    }
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
