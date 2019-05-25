<template>
  <FormItem :label="title">
    <Select v-model="_value" transfer>
      <Option v-for="(item, i) in list" :value="item[0]" :key="i">{{ item[1] || item[0] }}</Option>
    </Select>
  </FormItem>
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
  computed: {
    parent() {
      return this.$getParent('UciSection');
    },
    config() {
      return this.parent.config;
    },
    sid() {
      return this.parent.sid;
    },
    _value: {
      get() {
        if (this.value)
          return this.value;

        if (!this.name)
          return '';

        if (!this.config || !this.sid)
          return '';
        return this.$uci.get(this.config, this.sid, this.name);
      },
      set(newValue) {
        if (!this.name)
          return;
        this.$uci.set(this.config, this.sid, this.name, newValue);
      }
    }
  }
}
</script>