<template>
  <FormItem :label="title">
    <slot :_value="_value"></slot>
  </FormItem>
</template>

<script>

export default {
  name: 'UciValue',
  props: {
    name: String,
    title: String,
    value: String,
    defaultVal: String
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
        return this.$uci.get(this.config, this.sid, this.name) || this.defaultVal;
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
