<template>
  <div>
    <slot></slot>
    <div>
      <Button type="primary" style="margin-right: 10px" @click="apply">Save &  Apply</Button>
      <Button type="warning" @click="reset">Reset</Button>
    </div>
  </div>
</template>

<script>
export default {
  name: 'UciMap',
  props: {
    config: String
  },
  methods: {
    apply() {
      this.$uci.save().then(() => {
        this.$uci.apply().then(() => {
          this.load();
          this.$Message.success('Save & Apply successfully');
        }).catch(e => {
          if (e.code === 5)
            this.$Message.warning('There are no changes to apply');
          else
            throw e;
        });
      });
    },
    reset() {
      this.$uci.reset();
      this.$bus.$emit(this.config + '-reset');
    },
    load() {
      this.$uci.load(this.config).then(() => {
        this.$bus.$emit(this.config + '-loaded');
      });
    }
  },
  mounted() {
    this.load();
  },
  destroyed() {
    this.$uci.reset();
  }
}
</script>

