<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :depends="depends" :load="load" :save="save" type="list" :options="options" @change="onChange"></uci-value>
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
    options: Array,
    load: Function,
    save: Function,
    depends: [String, Object]
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