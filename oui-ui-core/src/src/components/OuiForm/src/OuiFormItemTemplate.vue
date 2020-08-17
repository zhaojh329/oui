<template>
  <a-form-model-item v-if="visible" ref="a-form-model-item" :prop="prop" :rules="rules" :label="label" :help="help">
    <slot></slot>
  </a-form-model-item>
</template>

<script>
export default {
  name: 'OuiFormItemTemplate',
  props: {
    visible: Boolean,
    label: String,
    prop: String,
    help: String,
    rules: [Array, Object]
  }
}
</script>
