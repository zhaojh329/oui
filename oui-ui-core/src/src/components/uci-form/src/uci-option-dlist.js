import mixin from './uci-option'
import UciDlist from './uci-dlist'

export default {
  name: 'UciOptionDlist',
  mixins: [mixin],
  props: {
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: Array,
    suggestions: Array
  },
  components: {
    UciDlist
  },
  methods: {
    convertFromUCI(value) {
      if (typeof(value) === 'undefined')
        value = [];
      return value;
    },
    view(prop) {
      return <uci-dlist prop={prop} v-model={this.form[prop]} suggestions={this.suggestions}/>;
    }
  }
}
