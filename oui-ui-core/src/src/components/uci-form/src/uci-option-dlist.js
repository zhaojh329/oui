import mixin from './uci-option'

export default {
  name: 'UciOptionDlist',
  mixins: [mixin],
  props: {
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: Array
  },
  methods: {
    convertFromUCI(value) {
      if (typeof(value) === 'undefined')
        value = [];
      return value;
    },
    renderOpt() {
      return 'uci-dlist';
    }
  }
}
