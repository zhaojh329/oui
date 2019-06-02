<template>
  <uci-value :tab="tab" :label="label" :name="name" :value="value" :initial="initial" :required="required" type="input" :placeholder="placeholder" :readonly="readonly"></uci-value>
</template>

<script>
import UciValue from './UciValue.vue'

export default {
  name: 'UciInput',
  props: {
    tab: String,
    label: String,
    name: String,
    value: String,
    initial: String,
    required: Boolean,
    placeholder: String,
    readonly: Boolean
  },
  components: {
    'UciValue': UciValue
  }
}
</script>