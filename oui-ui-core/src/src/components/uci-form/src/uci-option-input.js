import mixin from './uci-option'

export default {
  name: 'UciOptionInput',
  mixins: [mixin],
  props: {
    placeholder: String,
    password: Boolean
  },
  methods: {
    renderOpt(h, attrs, props) {
      attrs.placeholder = this.placeholder;
      props.showPassword = this.password;

      return 'el-input';
    }
  }
}
