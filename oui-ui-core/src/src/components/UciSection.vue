<template>
  <el-card :header="title">
    <slot></slot>
  </el-card>
</template>

<script>
export default {
  name: 'UciSection',
  props: {
    name: String,
    title: String,
    typed: Boolean
  },
  data() {
    return {
      sid: '',
      uciValues: {}
    }
  },
  computed: {
    config() {
      return this.$getParent('UciForm').config;
    },
    loaded() {
      return this.$getParent('UciForm').loaded;
    }
  },
  watch: {
    loaded(n) {
      if (n > 1)
        return;

      if (this.typed) {
        const values = this.$uci.get(this.config);
        Object.keys(values).forEach(sid => {
          const s = values[sid];
          if (s['.type'] === this.name)
            this.sid = sid;
        });
      } else {
        this.sid = this.name;
      }
    }
  }
}
</script>