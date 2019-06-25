import mixin from './uci-option'

export default {
  name: 'UciOptionInput',
  mixins: [mixin],
  props: {
    placeholder: String,
    password: Boolean
  },
  methods: {
    view(prop) {
      return <el-input placeholder={this.placeholder} show-password={this.password} v-model={this.form[prop]} />;
    }
  }
}
