export default {
  name: 'UciTab',
  inject: ['uciForm', 'uciSection'],
  props: {
    title: String,
    name: String
  },
  computed: {
    isTableExpand() {
      let parent = this.$getParent('UciTableExpand');
      if (parent)
        return true;
      return false;
    },
    options() {
      if (this.isTableExpand)
        return this.uciSection.arrayedTableExpandOptions.filter(o => o.tabName === this.name);
      else
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
