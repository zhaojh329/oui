<template>
  <FormItem :label="title">
    <Input v-model="_value" />
  </FormItem>
</template>

<script>

export default {
  name: 'UciInputValue',
  props: {
    name: String,
    title: String,
    value: String
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