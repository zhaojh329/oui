<template>
  <UciValue :title="title" :name="name" :value="value" v-slot="{_value}" ref="UciValue">
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
    readonly: Boolean
  },
  methods: {
    onChange(event) {
      this.$refs.UciValue._value = event.target.value;
    }
  }
}
</script>