import UciDlist from './uci-dlist'

export default {
  name: 'UciOptionUi',
  inject: ['uciForm'],
  props: {
    label: String,
    table: Boolean,
    option: Object,
    sid: String
  },
  components: {
    UciDlist
  },
  computed: {
    form() {
      return this.uciForm.form;
    },
    prop() {
      return this.option.formProp(this.sid);
    }
  },
  render(h) {
    const self = this;
    const data = {
      props: {
        value: this.form[this.prop],
        prop: this.prop
      },
      on: {
        input: value => {
          this.form[this.prop] = value;
        }
      }
    };
    const children = [];

    let opt;

    if (this.option.$scopedSlots.default) {
      opt = this.option.$scopedSlots.default({sid: this.sid, o: this.option});
    } else {
      const tag = this.option.renderOpt(h, data, children);
      opt = h(tag, data, children);
    }

    return h('el-form-item', {
      props: {
        label: self.label,
        prop: self.prop,
        labelWidth: self.table ? 'auto' : ''
      }
    }, [opt]);
  }
}
