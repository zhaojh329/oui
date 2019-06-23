<template>
  <div class="uci-form">
    <div class="hidden-Section"><slot></slot></div>
    <el-form class="form" label-width="280px" ref="form" :model="form" :rules="rules" @validate="onValidate" :disabled="readonly">
      <el-tabs v-if="tabbed" value="0">
        <el-tab-pane v-for="(s, i) in sections" :key="i" :name="i + ''" :label="s.title">
          <el-table v-if="s.table" :data="s.sids">
            <el-table-column v-for="o in s.arrayedOptions" :key="o.name" :label="o.label" :width="o.width">
              <template slot="header" v-if="o.header">
                <span v-html="o.header"></span>
              </template>
              <template v-slot="{ row }">
                <uci-form-item :option="o" :sid="row" :form="form" table></uci-form-item>
              </template>
            </el-table-column>
            <el-table-column v-if="s.addable && s.type && !s.name" :width="s.tableActionWidth">
              <template v-slot="{ row }">
                <uci-section-table-action-ui style="margin-bottom: 22px" :section="s" :sid="row"></uci-section-table-action-ui>
              </template>
            </el-table-column>
          </el-table>
          <el-collapse v-else-if="s.collapse" v-model="activeCollapseItem" accordion>
            <el-collapse-item v-for="sid in s.sids" :key="sid" :title="sid" :name="sid">
              <template slot="title">
                <span v-if="activeCollapseItem === sid"></span>
                <span v-else>
                  <el-badge :key="'badge' + i" v-if="s.getErrorNum(sid) > 0" :value="s.getErrorNum(sid)"></el-badge>
                  <template v-for="(t, i) in s.teasersValue(sid)">
                    {{ t[0] }}: <strong :key="i">{{ t[1] }}</strong>{{ t[2] ? '' : ' | ' }}
                  </template>
                </span>
              </template>
              <uci-section-ui :sestion="s" :sid="sid"></uci-section-ui>
            </el-collapse-item>
          </el-collapse>
          <div v-else v-for="(sid, i) in s.sids" :key="sid">
            <uci-section-ui :sestion="s" :sid="sid" :divider="i < s.sids.length - 1"></uci-section-ui>
          </div>
          <uci-section-add :sestion="s"></uci-section-add>
        </el-tab-pane>
      </el-tabs>
      <template v-else v-for="(s, i) in sections">
        <el-card :key="i" :header="s.title">
          <el-table v-if="s.table" :data="s.sids">
            <el-table-column v-for="o in s.arrayedOptions" :key="o.name" :label="o.label" :width="o.width">
              <template slot="header" v-if="o.header">
                <span v-html="o.header"></span>
              </template>
              <template v-slot="{ row }">
                <uci-form-item :option="o" :sid="row" :form="form" table></uci-form-item>
              </template>
            </el-table-column>
            <el-table-column v-if="s.addable && s.type && !s.name" :width="s.tableActionWidth">
              <template v-slot="{ row }">
                <uci-section-table-action-ui style="margin-bottom: 22px" :section="s" :sid="row"></uci-section-table-action-ui>
              </template>
            </el-table-column>
          </el-table>
          <el-collapse v-else-if="s.collapse" v-model="activeCollapseItem" accordion>
            <el-collapse-item v-for="sid in s.sids" :key="sid" :title="sid" :name="sid">
              <template slot="title">
                <span v-if="activeCollapseItem === sid"></span>
                <span v-else>
                  <el-badge :key="'badge' + i" v-if="s.getErrorNum(sid) > 0" :value="s.getErrorNum(sid)"></el-badge>
                  <template v-for="(t, i) in s.teasersValue(sid)">
                    {{ t[0] }}: <strong :key="i">{{ t[1] }}</strong>{{ t[2] ? '' : ' | ' }}
                  </template>
                </span>
              </template>
              <uci-section-ui :sestion="s" :sid="sid"></uci-section-ui>
            </el-collapse-item>
          </el-collapse>
          <div v-else v-for="(sid, i) in s.sids" :key="sid">
            <uci-section-ui :sestion="s" :sid="sid" :divider="i < s.sids.length - 1"></uci-section-ui>
          </div>
          <uci-section-add :sestion="s"></uci-section-add>
        </el-card>
      </template>
    </el-form>
    <el-button-group class="apply-btn">
      <el-button :disabled="readonly" type="primary" @click="apply">{{ $t('Save & Apply') }}</el-button>
      <el-button :disabled="readonly" type="warning" @click="reset">{{ $t('Reset') }}</el-button>
    </el-button-group>
  </div>
</template>

<script>
import UciFormItem from './uci-form-item'
import UciSectionAdd from './uci-section-add'
import UciSectionUi from './uci-section-ui'
import UciSectionTableActionUi from './uci-section-table-action-ui'

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
    tabbed: Boolean
  },
  components: {
    UciFormItem,
    UciSectionAdd,
    UciSectionUi,
    UciSectionTableActionUi
  },
  data() {
    return {
      loaded: false, /* Indicates whether the data is loaded */
      uid: 0, /* Used for unique identification of each uci-option vue instance */
      form: {},
      rules: {},
      validates: {},
      sections: [], /* UciSection vue instances */
      activeCollapseItem: ''
    }
  },
  computed: {
    readonly() {
      return !this.$uci.writable(this.config);
    }
  },
  methods: {
    getUID() {
      return this.uid++;
    },
    load() {
      return this.$uci.load(this.config);
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
                this.$uci.apply().then(() => {
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
              this.$emit('apply');
              loading.close();
              this.$message.success(this.$t('Configuration has been applied'));
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

      this.activeCollapseItem = '';

      this.sections.forEach(s => {
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
}
</style>
