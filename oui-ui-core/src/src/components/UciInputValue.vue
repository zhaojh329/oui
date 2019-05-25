<template>
  <FormItem :label="title">
    <Input v-model="value" />
  </FormItem>
</template>

<script>
export default {
  name: 'UciInputValue',
  props: {
    name: String,
    title: String
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
    value: {
      get() {
        if (!this.config || !this.sid)
          return '';
        return this.$uci.get(this.config, this.sid, this.name);
      },
      set(newValue) {
        this.$uci.set(this.config, this.sid, this.name, newValue);
      }
    }
  }
}
</script>