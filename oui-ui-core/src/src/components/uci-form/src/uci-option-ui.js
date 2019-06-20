import UciDlist from './uci-dlist'

export default {
  name: 'UciOptionUi',
  inject: ['uciForm'],
  props: {
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

    if (this.option.$scopedSlots.default)
      return this.option.$scopedSlots.default({sid: this.sid, o: this.option});

    const tag = this.option.renderOpt(h, data, children);

    return h(tag, data, children);
  }
}
