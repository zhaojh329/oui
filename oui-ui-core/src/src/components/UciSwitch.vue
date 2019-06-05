<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :depends="depends" :load="load" :save="save" type="switch" :active-value="activeValue" :inactive-value="inactiveValue" @change="onChange"></uci-value>
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
    save: Function,
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
  },
  methods: {
    onChange(data) {
      this.$emit('change', data);
    }
  }
}
</script>