<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :on-save="onSave" type="switch"></uci-value>
</template>

<script>
import UciValue from './UciValue.vue'

export default {
  name: 'UciInput',
  props: {
    label: String,
    name: String,
    value: String,
    initial: String,
    onSave: Function
  },
  components: {
    'UciValue': UciValue
  }
}
</script>