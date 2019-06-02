<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" type="switch"></uci-value>
</template>

<script>
import UciValue from './UciValue.vue'

export default {
  name: 'UciInput',
  props: {
    label: String,
    name: String,
    value: String,
    initial: String
  },
  components: {
    'UciValue': UciValue
  }
}
</script>