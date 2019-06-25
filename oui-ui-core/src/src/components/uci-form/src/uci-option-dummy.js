import mixin from './uci-option'

export default {
  name: 'UciOptionDummy',
  mixins: [mixin],
  methods: {
    view(prop) {
      return <span>{this.form[prop]}</span>;
    },
    _save() {
    },
    _apply() {
    }
  }
}
