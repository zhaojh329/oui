<template>
  <UciValue :title="title" :name="name" :value="value" :default-val="defaultVal" v-slot="{_value}" ref="UciValue">
    <Select :value="_value" transfer @on-change="onChange">
      <Option v-for="item in list" :value="item[0]" :key="item[0]">{{ item[1] || item[0] }}</Option>
    </Select>
  </UciValue>
</template>

<script>

export default {
  name: 'UciListValue',
  props: {
    name: String,
    title: String,
    value: String,
    defaultVal: String,
    list: Array
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