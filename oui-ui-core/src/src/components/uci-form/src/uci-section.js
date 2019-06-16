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
    /* The type prop must be provided for add or delete */
    addable: Boolean,
    /* If false, the add operation will add a named section */
    anonymous: {
      type: Boolean,
      default: true
    },
    /* Render in a table */
    table: Boolean,
    filter: Function,
    option: {
      type: Object,
      default: () => {}
    },
    add: Function,
    collabsible: {
      type: Boolean,
      default: true
    },
    teasers: Array
  },
  data() {
    return {
      tabs: [], /* UciTab instances */
      options: [], /* UciOption instances */
      sections: [], /* uci sections */
      nsid: '' /* Added sid, waiting to be rendered */
    }
  },
  computed: {
    loaded() {
      return this.uciForm.loaded;
    },
    uciSections() {
      let sections = [];

      if (this.name)
        sections = this.sections.filter(s => s['.name'] === this.name);
      else if (this.type)
        sections = this.sections;

      if (this.filter)
        sections = sections.filter(s => this.filter(this, s));
      return sections;
    },
    config() {
      return this.uciForm.config;
    },
    allOptions() {
      const options = [...this.options];
      this.tabs.forEach(tab => {
        options.push(...tab.options);
      });
      return options;
    }
  },
  watch: {
    loaded() {
      this.load();
    },
    sections() {
      this.buildForm();
    }
  },
  created() {
    this.uciForm.sections.push(this);
  },
  methods: {
    load() {
      this.sections = this.$uci.sections(this.config, this.type);
    },
    buildFormOptions(options, sid) {
      options.forEach(o => {
        o.buildForm(sid);
      });
    },
    buildForm() {
      const uciSections = this.uciSections;
      uciSections.forEach(uciSection => {
        const sid = uciSection['.name'];
        if (this.nsid !== '' && sid !== this.nsid)
          return;
        this.buildFormOptions(this.options, sid);
        this.tabs.forEach(tab => {
          this.buildFormOptions(tab.options, sid);
        });
      });
      this.nsid = '';
    },
    delForm(sid) {
      for (const prop in this.uciForm.form) {
        if (prop.match(sid))
          this.$delete(this.uciForm.form, prop);
      }
    },
    findOption(name) {
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
    del(sid) {
      this.$uci.del(this.config, sid);
      this.delForm(sid);
      this.load();
    },
    teasersValue(sid) {
      const teasers = [];

      if (this.teasers) {
        this.teasers.forEach(t => {
          const o = this.findOption(t);
          teasers.push([o.label, o.formValue(sid)]);
        });
      } else {
        this.options.forEach(o => {
          teasers.push([o.label, o.formValue(sid)]);
        });

        this.tabs.forEach(tab => {
          tab.options.forEach(o => {
            teasers.push([o.label, o.formValue(sid)]);
          });
        });
      }

      teasers[teasers.length - 1].push('end');

      return teasers;
    },
    getErrorNum(sid) {
      const validates = this.uciForm.validates;
      const keys = Object.keys(validates).filter(key => {
        return sid === validates[key].sid && !validates[key].valid;
      });

      return keys.length;
    }
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
