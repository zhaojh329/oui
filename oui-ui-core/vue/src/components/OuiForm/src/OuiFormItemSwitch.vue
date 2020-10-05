<template>
  <oui-form-item-template v-bind="OuiFormItemTemplateProps">
    <a-switch v-model="model"/>
  </oui-form-item-template>
</template>

<script>
import OuiFormItemMixin from './OuiFormItemMixin'

export default {
  name: 'OuiFormItemSwitch',
  mixins: [OuiFormItemMixin],
  props: {
    initial: Boolean,
    trueValue: {
      type: [String, Boolean, Number],
      default: '1'
    },
    falseValue: {
      type: [String, Boolean, Number],
      default: '0'
    }
  },
  computed: {
    dependExprValue () {
      return `${this.model}`
    }
  },
  methods: {
    convertUciValue (value) {
      if (typeof value !== 'boolean') return value === this.trueValue
      return value
    },
    __save () {
      if (this.changed()) {
        if (this.save) { return this.save(this) }

        if (this.model) {
          this.$uci.set(this.config, this.sid, this.name, this.trueValue)
        } else {
          this.$uci.set(this.config, this.sid, this.name, this.falseValue)
        }
      }
    }
  }
}
</script>
