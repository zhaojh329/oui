<template>
  <UciValue :title="title" :name="name" :value="value" :default-val="defaultVal" v-slot="{_value}" ref="UciValue">
    <Input :value="_value" :readonly="readonly" @on-change="onChange" />
  </UciValue>
</template>

<script>

export default {
  name: 'UciInputValue',
  props: {
    name: String,
    title: String,
    value: String,
    defaultVal: String,
    readonly: Boolean
  },
  methods: {
    onChange(event) {
      this.$refs.UciValue._value = event.target.value;
      this.$emit('on-change', {
        config: this.$refs.UciValue.config,
        sid: this.$refs.UciValue.sid,
        name: this.name,
        value: event.target.value
      });
    }
  }
}
</script>