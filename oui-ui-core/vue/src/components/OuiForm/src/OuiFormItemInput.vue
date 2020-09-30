<template>
  <oui-form-item-template v-bind="OuiFormItemTemplateProps">
    <component :is="is" v-model="model" :type="type" :placeholder="placeholder" :prefix="prefixIcon" :suffix="suffixIcon"
    :addon-before="prepend" :addon-after="append"/>
  </oui-form-item-template>
</template>

<script>
import OuiFormItemMixin from './OuiFormItemMixin'

export default {
  name: 'OuiFormItemInput',
  mixins: [OuiFormItemMixin],
  props: {
    placeholder: String,
    prepend: String,
    append: String,
    type: {
      validator (value) {
        return ['text', 'url', 'email', 'date', 'number', 'tel'].indexOf(value) !== -1
      },
      default: 'text'
    },
    password: Boolean,
    prefixIcon: String,
    suffixIcon: String
  },
  computed: {
    is () {
      if (this.password) return 'a-input-password'
      return 'a-input'
    }
  }
}
</script>
