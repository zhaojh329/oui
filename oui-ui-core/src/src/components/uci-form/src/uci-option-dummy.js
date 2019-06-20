import mixin from './uci-option'

export default {
  name: 'UciOptionDummy',
  mixins: [mixin],
  methods: {
    renderOpt(h, data, children) {
      children.push(data.props.value);
      return 'span';
    },
    _save() {
    }
  }
}
