<template>
  <oui-form-item-template v-bind="OuiFormItemTemplateProps">
    <slot :self="this"></slot>
  </oui-form-item-template>
</template>

<script>
import OuiFormItemMixin from './OuiFormItemMixin.vue'

export default {
  name: 'OuiFormItem',
  mixins: [OuiFormItemMixin]
}
</script>
