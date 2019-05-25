<template>
  <UciValue :title="title" :name="name" :value="value" v-slot="{_value}" ref="UciValue">
    <Select :value="_value" transfer @on-change="onChange">
      <Option v-for="(item, i) in list" :value="item[0]" :key="i">{{ item[1] || item[0] }}</Option>
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
    list: Array
  },
  methods: {
    onChange(value) {
      this.$refs.UciValue._value = value;
    }
  }
}
</script>