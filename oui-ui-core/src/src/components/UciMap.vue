<template>
  <div>
    <slot></slot>
    <div style="position:absolute; right: 100px">
      <Button type="primary" style="margin-right: 10px" @click="apply">Save &  Apply</Button>
      <Button type="info" style="margin-right: 10px" @click="save">Save</Button>
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
          this.$Message.success('Apply successfully');
        });
      });
    },
    save() {
      this.$uci.save().then(() => {
        this.$Message.success('Save successfully');
      });
    },
    reset() {

    }
  },
  mounted() {
    this.$uci.load(this.config).then(() => {
      this.$bus.$emit(this.config + '-loaded');
    });
  }
}
</script>