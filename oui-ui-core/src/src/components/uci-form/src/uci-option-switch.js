import mixin from './uci-option'

export default {
  name: 'UciOptionSwitch',
  mixins: [mixin],
  props: {
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: {
      type: [String, Number, Boolean],
      default: () => undefined
    },
    activeValue: {
      type: [String, Number, Boolean],
      default: () => undefined
    },
    inactiveValue: {
      type: [String, Number, Boolean],
      default: () => undefined
    }
  },
  methods: {
    convertFromUCI(value) {
      let inactiveValue = false;
      if (typeof(this.inactiveValue) !== 'undefined')
        inactiveValue = this.inactiveValue;

      if (typeof(this.activeValue) !== 'undefined') {
        if (value !== this.activeValue)
          value = inactiveValue;
      } else {
        if (value && value !== '0')
          value = true;
        else
          value = inactiveValue;
      }

      return value;
    },
    dependExprValue(sid) {
      let value = this.formValue(sid);
      let r;

      if (typeof(this.activeValue) !== 'undefined')
        r = value === this.activeValue;
      else
        r = !!value;

      return r.toString();
    },
    renderOpt(h, attrs, props) {
      props.activeValue = this.activeValue;
      props.inactiveValue = this.inactiveValue;

      return 'el-switch';
    }
  }
}
