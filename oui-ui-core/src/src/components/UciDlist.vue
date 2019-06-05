<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :depends="depends" :load="load" :on-save="onSave" type="dlist" :options="options"></uci-value>
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
    load: Function,
    onSave: Function,
    depends: [String, Object]
  },
  components: {
    'UciValue': UciValue
  }
}
</script>