import mixin from './uci-option'

export default {
  name: 'UciOptionList',
  mixins: [mixin],
  props: {
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: [String, Array],
    options: {
      type: Array,
      default() {
        return [];
      }
    },
    multiple: Boolean,
    allowCreate: Boolean
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
      } else if (typeof(value) !== 'undefined') {
        value = value.toString();
      }
      return value;
    },
    convertToUCI(value) {
      if (this.multiple)
        value = value.join(' ');
      return value;
    },
    renderOpt(h, attrs, props, children) {
      props.clearable = !this.required;
      props.multiple = this.multiple;
      props.filterable = true;
      props.allowCreate = this.allowCreate;

      this.transformedOptions.forEach(o => {
        const dom = h('el-option', {
          props: {
            value: o[0],
            label: o[1]
          }
        });
        children.push(dom);
      });

      return 'el-select';
    }
  }
}
