<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :depends="depends" :load="load" :on-save="onSave" type="switch" :active-value="activeValue" :inactive-value="inactiveValue"></uci-value>
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
    load: Function,
    onSave: Function,
    depends: [String, Object],
    activeValue: {
      type: String,
      default: '1'
    },
    inactiveValue: {
      type: String,
      default: '0'
    }
  },
  components: {
    'UciValue': UciValue
  }
}
</script>