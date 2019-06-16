export default {
  name: 'UciTab',
  inject: ['uciForm', 'uciSection'],
  props: {
    title: String,
    name: String
  },
  data() {
    return {
      options: [] /* UciOption instances */
    }
  },
  methods: {
    getErrorNum(sid) {
      const validates = this.uciForm.validates;
      const keys = Object.keys(validates).filter(key => {
        return sid === validates[key].sid && validates[key].tab === this.name && !validates[key].valid;
      });

      return keys.length;
    }
  },
  watch: {
    options() {
      this.uciSection.buildForm();
    }
  },
  created() {
    this.uciSection.tabs.push(this);
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
