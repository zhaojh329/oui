import mixin from './uci-option'

export default {
  name: 'UciOptionDummy',
  mixins: [mixin],
  methods: {
    renderOpt(h, attrs, props, children) {
      children.push(props.value);
      return 'span';
    },
    saveUCI() {
    },
    applyUCI() {
      return null;
    }
  }
}
