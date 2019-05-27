<template>
  <Card style="margin-bottom: 15px" :title="title">
    <Form :label-width="200">
      <slot></slot>
    </Form>
  </Card>
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
      sid: ''
    }
  },
  computed: {
    parent() {
      return this.$getParent('UciMap');
    },
    config() {
      return this.parent.config;
    }
  },
  mounted() {
    this.$bus.$on(this.config + '-loaded', () => {
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
    });
  }
}
</script>