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
      initials: {},
      validated: {},
      saveFuns: {}
    }
  },
  methods: {
    saveCustom(uciApplied, loading) {
      const promises = [];

      Object.keys(this.saveFuns).forEach(key => {
        const save = this.saveFuns[key];
        if (typeof(save) !== 'undefined') {
          const v = this.form[key];
          if (v !== this.initials[key]) {
            const p = new Promise(resolve => {
              save(resolve, v);
            });
            promises.push(p);
          }
        }
      });

      if (promises.length === 0) {
        loading.close();

        if (!uciApplied) {
          this.$message.warning('There are no changes to apply');
          return;
        }

        this.load(true);
        this.$message.success('Configuration has been applied');
        return;
      }

      Promise.all(promises).then(() => {
        loading.close();
        this.load(true);
        this.$message.success('Configuration has been applied');
      });
    },
    apply() {
      const loading = this.$loading({
        text: 'Waiting for configuration to be applied...'
      });

      this.$uci.save().then(() => {
        this.$uci.apply().then(() => {
          this.saveCustom(true, loading);
        }).catch(e => {
          if (e.code === 5)
            this.saveCustom(false, loading);
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
    addFormItem(item) {
      this.$set(this.form, item.name, '');
      this.$set(this.initials, item.name, '');
      this.$set(this.validated, item.name, {valid: true, tab: item.tab});
      this.$set(this.saveFuns, item.name, item.onSave);
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
