<template>
  <UciValue :title="title" :name="name" :value="value" :default-val="defaultVal" v-slot="{_value}" ref="UciValue">
    <i-switch :value="_value" true-value="1" false-value="0" @on-change="onChange"></i-switch>
  </UciValue>
</template>

<script>

export default {
  name: 'UciSwitchValue',
  props: {
    name: String,
    title: String,
    value: String,
    defaultVal: {
      type: String,
      default: '0'
    },
    readonly: Boolean
  },
  methods: {
    onChange(value) {
      this.$refs.UciValue._value = value;
      this.$emit('on-change', {
        config: this.$refs.UciValue.config,
        sid: this.$refs.UciValue.sid,
        name: this.name,
        value: value
      });
    }
  }
}
</script>