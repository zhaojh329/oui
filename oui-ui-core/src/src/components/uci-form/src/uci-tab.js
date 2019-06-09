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
  computed: {
    errorNum() {
      const validates = this.uciForm.validates;
      const keys = Object.keys(validates).filter(key => {
        return validates[key].tab === this.name && !validates[key].valid;
      });

      return keys.length;
    }
  },
  created() {
    this.uciSection.tabs.push(this);
  },
  render(h) {
    return h('div', this.$slots.default);
  }
}
