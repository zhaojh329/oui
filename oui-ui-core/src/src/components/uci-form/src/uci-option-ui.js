import UciDlist from './uci-dlist'

export default {
  name: 'UciOptionUi',
  inject: ['uciForm'],
  props: {
    option: Object,
    sid: String,
    value: [String, Array, Object, Boolean]
  },
  components: {
    UciDlist
  },
  computed: {
    prop() {
      return this.option.formProp(this.sid);
    }
  },
  render(h) {
    const self = this;
    const children = [];
    const attrs = {};
    const props = {
      value: this.value,
      prop: this.prop
    }

    if (this.option.$scopedSlots.default)
      return this.option.$scopedSlots.default(props);

    const tag = this.option.renderOpt(h, attrs, props, children);

    return h(tag, {
      attrs: attrs,
      props: props,
      on: {
        input: value => {
          self.$emit('input', value)
        }
      }
    }, children);
  }
}
