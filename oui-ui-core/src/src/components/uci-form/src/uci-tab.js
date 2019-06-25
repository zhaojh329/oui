export default {
  name: 'UciTab',
  inject: ['uciForm', 'uciSection'],
  props: {
    title: String,
    name: String
  },
  computed: {
    options() {
      return this.uciSection.arrayedOptions.filter(o => o.tabName === this.name);
    }
  },
  methods: {
    getErrorNum(sid) {
      return this.uciForm.getErrorNum(sid, this.name);
    }
  },
  created() {
    this.uciSection.tabs.push(this);
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
