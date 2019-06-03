<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :on-load="onLoad" :on-save="onSave" type="dlist" :options="options"></uci-value>
</template>

<script>
import UciValue from './UciValue.vue'

export default {
  name: 'UciDlist',
  props: {
    label: String,
    name: String,
    value: String,
    initial: String,
    options: Array,
    onLoad: Function,
    onSave: Function
  },
  components: {
    'UciValue': UciValue
  }
}
</script>