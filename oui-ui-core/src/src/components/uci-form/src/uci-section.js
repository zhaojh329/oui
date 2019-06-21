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
    /* Parameters: uci section, self */
    filter: Function,
    options: {
      type: Object,
      default: () => {}
    },
    /*
    ** Custom add function
    ** Parameters: self, name
    */
    add: Function,
    collabsible: {
      type: Boolean,
      default: true
    },
    teasers: Array
  },
  data() {
    return {
      loaded: false, /* Indicates whether the data is loaded */
      tabs: [], /* uci-tab vue instances */
      children: {}, /* uci-option vue instances */
      sections: [] /* uci sections */
    }
  },
  computed: {
    config() {
      return this.uciForm.config;
    },
    collapse() {
      return this.sids.length > 1 && this.collabsible;
    },
    arrayedOptions() {
      return Object.keys(this.children).map(name => this.children[name]).sort((a, b) => a.uid - b.uid);
    },
    sids() {
      let sections = [];

      if (this.name)
        sections = this.sections.filter(s => s['.name'] === this.name);
      else if (this.type)
        sections = this.sections;

      if (this.filter)
        sections = sections.filter(s => this.filter(s, this));
      return sections.map(s => s['.name']);
    }
  },
  watch: {
    'uciForm.loaded'() {
      this.load();
    }
  },
  methods: {
    addChild(o) {
      this.$set(this.children, o.name, o);
    },
    delChild(o) {
      const d = this.children[o.name];
      if (d && d.uid === o.uid)
        this.$delete(this.children, o.name);
    },
    load() {
      this.sections = this.$uci.sections(this.config, this.type);
      this.loaded = true;
    },
    buildForm(sid) {
      for (const name in this.children)
        this.children[name].buildForm(sid);
    },
    destroyForm(sid) {
      for (const name in this.children)
        this.children[name].destroyFormSid(sid);
    },
    del(sid) {
      this.$uci.del(this.config, sid);
      this.load();
      this.destroyForm(sid);
    },
    teasersValue(sid) {
      const teasers = [];

      if (this.teasers) {
        this.teasers.forEach(name => {
          const o = this.children[name];
          teasers.push([o.label, o.formValue(sid)]);
        });
      } else {
        for (const name in this.children) {
          const o = this.children[name];
          const v = o.formValue(sid)
          teasers.push([o.label, v]);
        }
      }

      teasers[teasers.length - 1].push('end');

      return teasers;
    },
    getErrorNum(sid, tab) {
      const validates = this.uciForm.validates;
      const keys = Object.keys(validates).filter(key => {
        const err = sid === validates[key].sid && !validates[key].valid;
        if (tab)
          return err && validates[key].tab === tab;
        return err;
      });

      return keys.length;
    },
    save() {
      this.sids.forEach(sid => {
        for (const name in this.children)
          this.children[name]._save(sid);
      });
    },
    apply() {
      const promises = [];
      this.sids.forEach(sid => {
        for (const name in this.children) {
          const p = this.children[name]._apply(sid);
          if (p)
            promises.push(p);
        }
      });
      return promises;
    }
  },
  created() {
    this.uciForm.sections.push(this);
    if (this.uciForm.loaded)
      this.load();
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
