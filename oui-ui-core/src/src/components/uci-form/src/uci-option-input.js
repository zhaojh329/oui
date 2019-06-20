import mixin from './uci-option'

export default {
  name: 'UciOptionInput',
  mixins: [mixin],
  props: {
    placeholder: String,
    password: Boolean
  },
  methods: {
    renderOpt(h, data) {
      data.attrs = {};
      data.attrs.placeholder = this.placeholder;
      data.props.showPassword = this.password;

      return 'el-input';
    }
  }
}
