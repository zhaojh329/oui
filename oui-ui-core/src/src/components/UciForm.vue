<template>
  <div class="uci-form">
    <el-form class="form" label-width="250px" label-position="left" ref="form" :model="form" @validate="onValidate">
      <slot></slot>
    </el-form>
    <el-button-group class="apply-btn">
      <el-button type="primary" @click="apply">Save & Apply</el-button>
      <el-button type="warning" @click="reset">Reset</el-button>
    </el-button-group>
  </div>
</template>

<script>
export default {
  name: 'UciForm',
  props: {
    config: String
  },
  data() {
    return {
      loaded: 0,
      form: {},
      validated: {}
    }
  },
  methods: {
    apply() {
      this.$uci.save().then(() => {
        this.$uci.apply().then(() => {
          this.load(true);
          this.$message.success('Save & Apply successfully');
        }).catch(e => {
          if (e.code === 5)
            this.$message.warning('There are no changes to apply');
          else
            throw e;
        });
      });
    },
    reset() {
      this.$uci.reset();
      this.$refs['form'].resetFields();

      Object.keys(this.validated).forEach(key => {
        this.validated[key].valid = true;
      });

      this.loaded++;
    },
    load(force) {
      if (typeof(this.config) !== 'undefined') {
        this.$uci.load(this.config, force).then(() => {
          this.loaded++;
        });
      }
    },
    addFormItem(name, tab) {
      this.$set(this.form, name, '');
      this.$set(this.validated, name, {valid: true, tab: tab});
    },
    onValidate(name, valid) {
      this.validated[name].valid = valid;
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

<style lang="scss">
.uci-form {
  .form {
    > * {
      margin-bottom: 20px;
    }
  }

  .apply-btn {
    float: right;
    margin-right: 50px;
  }
}
</style>
