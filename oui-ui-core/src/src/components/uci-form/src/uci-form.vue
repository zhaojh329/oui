<template>
  <div class="uci-form">
    <div class="hidden-Section"><slot></slot></div>
    <el-form class="form" label-width="200px" ref="form" :model="form" :rules="rules" @validate="onValidate">
      <template v-for="(s, i) in sections">
        <el-card :key="i" :header="s.title">
          <div v-for="(u, i) in s.uciSections" :key="u['.name']">
            <uci-section-del :sestion="s" :sid="u['.name']"></uci-section-del>
            <el-tabs v-if="s.tabs.length > 0" :value="s.tabs[0].name">
              <el-tab-pane v-for="tab in s.tabs" :key="tab.name" :name="tab.name">
                <span slot="label">
                  {{ tab.title }}<el-badge v-if="tab.errorNum > 0" :value="tab.errorNum"></el-badge>
                </span>
                <uci-form-item v-for="o in tab.options" :key="o.name" :option="o" :sid="u['.name']" :form="form" :ref="o.prop(u['.name'])"></uci-form-item>
                <el-divider v-if="i < s.uciSections.length - 1" :key="'divider-' + u['.name']"></el-divider>
              </el-tab-pane>
            </el-tabs>
            <uci-form-item v-for="o in s.options" :key="o.name" :option="o" :sid="u['.name']" :form="form" :ref="o.prop(u['.name'])"></uci-form-item>
            <el-divider v-if="i < s.uciSections.length - 1" :key="'divider-' + u['.name']"></el-divider>
          </div>
          <uci-section-add :sestion="s" :form="form"></uci-section-add>
        </el-card>
      </template>
    </el-form>
    <el-button-group class="apply-btn">
      <el-button type="primary" @click="apply">Save & Apply</el-button>
      <el-button type="warning" @click="reset">Reset</el-button>
    </el-button-group>
  </div>
</template>

<script>
import UciSectionAdd from './uci-section-add'
import UciSectionDel from './uci-section-del'

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
    }
  },
  components: {
    UciSectionAdd,
    UciSectionDel
  },
  data() {
    return {
      loaded: 0,
      form: {},
      rules: {},
      validates: {},
      sections: [] /* UciSection instances */
    }
  },
  methods: {
    loading(text) {
      return this.$loading({
        text: text,
        spinner: 'el-icon-loading',
        background: 'rgba(0, 0, 0, 0.7)'
      });
    },
    load(force) {
      return this.$uci.load(this.config, force);
    },
    buildFormOptions(options, uciSection) {
      const sid = uciSection['.name'];

      for (const prop in this.form) {
        if (prop.match(sid))
          return;
      }

      options.forEach(o => {
        o.buildForm(sid);
      });
    },
    buildForm() {
      this.sections.forEach(s => {
        if (s.formBuilt)
          return;
        s.formBuilt = true;

        const uciSections = s.uciSections;
        uciSections.forEach(uciSection => {
          this.buildFormOptions(s.options, uciSection);
          s.tabs.forEach(tab => {
            this.buildFormOptions(tab.options, uciSection);
          });
        });
      });
    },
    onValidate(name, valid) {
      if (this.validates[name])
        this.validates[name].valid = valid;
    },
    saveOptions(options, uciSection) {
      const sid = uciSection['.name'];
      options.forEach(o => {
        o.saveUCI(sid);
      });
    },
    save() {
      this.sections.forEach(s => {
        const uciSections = s.uciSections;
        uciSections.forEach(uciSection => {
          this.saveOptions(s.options, uciSection);
          s.tabs.forEach(tab => {
            this.saveOptions(tab.options, uciSection);
          });
        });
      });
    },
    applyOptions(options, uciSection) {
      const promises = [];
      const sid = uciSection['.name'];
      options.forEach(o => {
        const p = o.applyUCI(sid);
        if (p)
          promises.push(p);
      });
      return promises;
    },
    applyAllOptions() {
      const allPromises = [];

      this.sections.forEach(s => {
        const uciSections = s.uciSections;
        uciSections.forEach(uciSection => {
          const promises = this.applyOptions(s.options, uciSection);
          allPromises.push(...promises);
          s.tabs.forEach(tab => {
            const promises = this.applyOptions(tab.options, uciSection);
            allPromises.push(...promises);
          });
        });
      });
      return allPromises;
    },
    apply() {
      this.$refs['form'].validate(valid => {
        if (!valid)
          return;

        this.save();

        const promises = this.applyAllOptions();

        if (this.$uci.changed() > 0) {
          const p = new Promise(resolve => {
            this.$uci.save().then(() => {
              this.$uci.apply().then(() => {
                resolve();
              });
            });
          });
          promises.push(p);
        }

        if (promises.length === 0) {
          this.$message.warning('There are no changes to apply');
          return;
        }

        const loading = this.loading('Waiting for configuration to be applied...');

        Promise.all(promises).then(() => {
          loading.close();
          this.load(true).then(() => {
            this.$message.success('Configuration has been applied');
          });
        });
      });
    },
    reset() {
      this.$uci.reset();
      this.loaded++;
    }
  },
  created() {
    const loading = this.loading('Loading...');
    this.load().then(() => {
      this.loaded++;
      loading.close();
    });
  },
  destroyed() {
    this.$uci.reset();
  }
}
</script>

<style lang="scss">
.uci-form {
  .hidden-Section {
    visibility: hidden;
  }

  .form {
    > * {
      margin-bottom: 20px;
    }
  }

  .apply-btn {
    float: right;
    margin-right: 50px;
  }

  .el-form-item {
    width: 60%;
  }

  .dlist {
    display: flex;
    flex-direction: column;

    > * {
      margin-bottom: 5px;
    }

    .el-button {
      width: 100px;
    }
  }
}
</style>
