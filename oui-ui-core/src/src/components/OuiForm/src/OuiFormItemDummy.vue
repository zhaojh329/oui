<template>
  <oui-form-item-template v-bind="OuiFormItemTemplateProps">
    <slot name="prepend"/>
    <span>{{ model }}</span>
    <slot name="append"/>
  </oui-form-item-template>
</template>

<script>
import OuiFormItemMixin from './OuiFormItemMixin.vue'

export default {
  name: 'OuiFormItemDummy',
  mixins: [OuiFormItemMixin],
  methods: {
    __save () {
    }
  }
}
</script>
