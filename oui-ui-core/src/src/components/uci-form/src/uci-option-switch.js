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
  data() {
    return {
      trueValues: ['1', 'yes', 'on', 'true', 'enabled', true]
    }
  },
  methods: {
    textValue(sid) {
      let v = this.formValue(sid);

      if (typeof(this.activeValue) !== 'undefined') {
        if (v === this.activeValue)
          return 'yes';
        else
          return 'no';
      } else {
        if (v)
          return 'yes';
        else
          return 'no';
      }
    },
    convertFromUCI(value) {
      let inactiveValue = false;

      if (typeof(this.inactiveValue) !== 'undefined')
        inactiveValue = this.inactiveValue;

      if (typeof(this.activeValue) !== 'undefined') {
        if (value !== this.activeValue)
          value = inactiveValue;
      } else {
        if (this.trueValues.indexOf(value) > -1)
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
    view(prop) {
      return <el-switch active-value={this.activeValue} inactive-value={this.inactiveValue} v-model={this.form[prop]} />;
    }
  }
}
