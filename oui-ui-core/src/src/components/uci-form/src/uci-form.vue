<template>
  <div>
    <div style="visibility: hidden"><slot></slot></div>
    <el-form ref="form" label-width="280px" :model="form" :rules="rules" :disabled="readonly" @validate="onValidate">
      <uci-form-body></uci-form-body>
    </el-form>
    <el-button-group style="margin-right: 50px; float: right">
      <el-button :disabled="readonly" type="primary" @click="apply">{{ $t('Save & Apply') }}</el-button>
      <el-button :disabled="readonly" type="warning" @click="reset">{{ $t('Reset') }}</el-button>
    </el-button-group>
  </div>
</template>

<script>
import UciFormBody from './uci-form-body'

export default {
  name: 'UciForm',
  provide() {
    return {
      uciForm: this
    };
  },
  props: {
    config: {
      type: String,
      required: true
    },
    tabbed: Boolean,
    afterLoaded: Function,
    applyTimeout: Number /* timeout for apply(second) */
  },
  components: {
    UciFormBody
  },
  data() {
    return {
      loaded: false, /* Indicates whether the data is loaded */
      uid: 0, /* Used for unique identification of each uci-option vue instance */
      form: {},
      rules: {},
      validates: {},
      sections: [], /* UciSection vue instances */
      readonly: true
    }
  },
  methods: {
    getUID() {
      return this.uid++;
    },
    getErrorNum(sid, tab) {
      const validates = this.validates;
      const keys = Object.keys(validates).filter(key => {
        const err = sid === key.split('_')[0] && !validates[key].valid;
        if (tab)
          return err && validates[key].tab === tab;
        return err;
      });

      return keys.length;
    },
    addProp(prop, data) {
      this.$set(this.form, prop, data.value);
      this.$set(this.rules, prop, data.rules);
      this.$set(this.validates, prop, {valid: true, tab: data.tab});
    },
    delProp(prop) {
      this.$delete(this.form, prop);
      this.$delete(this.rules, prop);
      this.$delete(this.validates, prop);
    },
    load() {
      return new Promise(resolve => {
        this.$uci.load(this.config).then(() => {
          this.readonly = !this.$uci.writable(this.config);
          if (this.afterLoaded)
            this.afterLoaded();
          resolve();
        });
      });
    },
    onValidate(name, valid) {
      if (this.validates[name])
        this.validates[name].valid = valid;
    },
    save() {
      return new Promise(resolve => {
        const promises = [];

        this.sections.forEach(s => {
          promises.push(...s.save());
        });

        if (promises.length > 0) {
          Promise.all(promises).then(() => {
            resolve();
          });
        } else {
          resolve();
        }
      });
    },
    apply() {
      this.$refs['form'].validate(valid => {
        if (!valid)
          return;

        this.save().then(() => {
          const promises = [];

          this.sections.forEach(s => {
            promises.push(...s.apply());
          });

          if (this.$uci.changed() > 0) {
            const p = new Promise(resolve => {
              this.$uci.save().then(() => {
                this.$uci.apply(this.applyTimeout).then(() => {
                  resolve();
                });
              });
            });
            promises.push(p);
          }

          if (promises.length === 0) {
            this.$message.warning(this.$t('There are no changes to apply'));
            return;
          }

          const loading = this.$getLoading(this.$t('Waiting for configuration to be applied...'));

          Promise.all(promises).then(() => {
            this.load().then(() => {
              this.reset();
              loading.close();
              this.$message.success(this.$t('Configuration has been applied'));

              this.$emit('applied');

              this.sections.forEach(s => {
                s.applied();
              });
            });
          });
        });
      });
    },
    reset() {
      this.form = {};
      this.rules = {};
      this.validates = {};
      this.$uci.reset();

      this.sections.forEach(s => {
        s.activeCollapseItem = '';
        s.load();
        s.buildForm();
      });
    }
  },
  created() {
    const loading = this.$getLoading(this.$t('Loading...'));
    this.load().then(() => {
      this.loaded = true;
      loading.close();
    });
  },
  destroyed() {
    this.$uci.reset();
  }
}
</script>
