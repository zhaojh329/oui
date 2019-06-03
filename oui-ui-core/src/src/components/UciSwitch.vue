<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :depends="depends" :on-load="onLoad" :on-save="onSave" type="switch"></uci-value>
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
    onLoad: Function,
    onSave: Function,
    depends: [String, Object]
  },
  components: {
    'UciValue': UciValue
  }
}
</script>