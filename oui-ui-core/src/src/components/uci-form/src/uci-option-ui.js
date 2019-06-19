import UciDlist from './uci-dlist'

export default {
  name: 'UciOptionUi',
  inject: ['uciForm'],
  props: {
    option: Object,
    sid: String,
    value: [String, Array]
  },
  components: {
    UciDlist
  },
  computed: {
    type() {
      return this.option.type;
    },
    tag() {
      const type = this.type;
      if (type === 'dummy')
        return 'span';

      if (type === 'input')
        return 'el-input';

      if (type === 'switch')
        return 'el-switch';

      if (type === 'list')
        return 'el-select';

      if (type === 'dlist')
        return 'uci-dlist';
    },
    prop() {
      return this.option.formProp(this.sid);
    }
  },
  render(h) {
    const type = this.type;
    const self = this;
    const children = [];
    const props = {
      value: this.value,
      prop: this.prop
    }

    if (type === 'input') {
      props.placeholder = this.option.placeholder;
      props.showPassword = this.option.password;
    } else if (type === 'dummy') {
      props.readonly = true;
      children.push(this.value);
    } else if (type === 'switch') {
      props.activeValue = this.option.activeValue;
      props.inactiveValue = this.option.inactiveValue;
    } else if (type === 'list') {
      props.clearable = !this.option.required;
      props.multiple = this.option.multiple;
      props.filterable = true;
      props.allowCreate = this.option.allowCreate;

      this.option.transformedOptions.forEach(o => {
        const dom = h('el-option', {
          props: {
            value: o[0],
            label: o[1]
          }
        });
        children.push(dom);
      });
    }

    if (this.option.$scopedSlots.default)
      return this.option.$scopedSlots.default(props);

    return h(this.tag, {
      props: props,
      on: {
        input: value => {
          self.$emit('input', value)
        }
      }
    }, children);
  }
}
