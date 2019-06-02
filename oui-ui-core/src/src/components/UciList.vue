<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" type="list" :options="options"></uci-value>
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
    options: Array
  },
  components: {
    'UciValue': UciValue
  }
}
</script>