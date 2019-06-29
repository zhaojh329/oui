import mixin from './uci-option'

export default {
  name: 'UciOptionList',
  mixins: [mixin],
  props: {
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: [String, Number, Array],
    options: {
      type: Array,
      default() {
        return [];
      }
    },
    multiple: Boolean,
    allowCreate: Boolean,
    exclude: [String, Function]
  },
  computed: {
    fixedOptions() {
      return this.options.map(o => {
        if (!Array.isArray(o))
          o = [o];
        o[0] = o[0] + '';
        if (o.length === 1)
          return [o[0], o[0]];
        return o;
      });
    }
  },
  methods: {
    convertFromUCI(value) {
      if (this.multiple) {
        if (typeof(value) === 'string')
          value = value.replace(/\s+/g, ' ').split(' ');
        else if (Array.isArray(value))
          value = value.map(v => v.toString());
        else
          value = [];
      } else {
        if (typeof(value) === 'undefined')
          value = '';
        value = value.toString();
      }
      return value;
    },
    convertToUCI(value) {
      if (this.multiple)
        value = value.join(' ');
      return value;
    },
    view(prop, sid) {
      const props = {
        filterable: true,
        clearable: !this.required,
        multiple: this.multiple,
        allowCreate: this.allowCreate
      }

      let options = this.fixedOptions;
      let exclude;

      if (typeof(this.exclude) === 'string')
        exclude = this.exclude;
      else if (typeof(this.exclude) === 'function')
        exclude = this.exclude(sid, this);

      if (exclude)
        options = options.filter(o => o[0] !== exclude);

      return <el-select style="width: 100%" props={props} v-model={this.form[prop]}>{options.map(o => <el-option value={o[0]} label={o[1]} />)}</el-select>;
    }
  }
}
