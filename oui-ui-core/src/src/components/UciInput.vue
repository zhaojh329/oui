<template>
  <uci-value :label="label" :name="name" :value="value" :initial="initial" :depends="depends" :load="load" :on-save="onSave" :required="required" type="input" :placeholder="placeholder" :readonly="readonly" @change="onChange"></uci-value>
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
    required: Boolean,
    placeholder: String,
    readonly: Boolean,
    load: Function,
    onSave: Function,
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