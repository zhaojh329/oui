export default {
  name: 'UciSection',
  provide() {
    return {
      uciSection: this
    };
  },
  inject: ['uciForm'],
  props: {
    /*
    ** The 'type' and 'name' must be provided at least one.
    ** If the name prop is provided, only the section whose name is equal to the name prop is rendered,
    ** otherwise render all sections whose type is equal to the type prop.
    */
    type: String,
    name: String,
    title: String,
    /* The type prop must be provided for add */
    addremove: Boolean
  },
  data() {
    return {
      formBuilt: false,
      tabs: [], /* UciTab instances */
      options: [], /* UciOption instances */
      sections: [] /* uci sections */
    }
  },
  computed: {
    loaded() {
      return this.uciForm.loaded;
    },
    uciSections() {
      if (this.name)
        return this.sections.filter(s => s['.name'] === this.name);
      if (this.type)
        return this.sections;
      return [];
    },
    config() {
      return this.uciForm.config;
    }
  },
  watch: {
    loaded() {
      this.load();
    }
  },
  created() {
    this.uciForm.sections.push(this);
  },
  methods: {
    load() {
      this.formBuilt = false;
      this.sections = this.$uci.sections(this.config, this.type);
      this.$nextTick(() => {
        this.uciForm.buildForm();
      });
    },
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
    },
    add() {
      this.$uci.add(this.config, this.name);
      this.load();
    }
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
