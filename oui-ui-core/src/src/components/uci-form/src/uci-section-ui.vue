<template>
  <div>
    <div class="oui-uci-section-ui">
      <div style="width: 100%">
        <el-tabs v-if="tabs.length > 0" :value="tabs[0].name">
          <template v-for="tab in tabs">
            <el-tab-pane v-if="tab.options.length > 0" :key="tab.name" :name="tab.name">
              <span slot="label">
                {{ tab.title }}<el-badge v-if="tab.getErrorNum(sid) > 0" :value="tab.getErrorNum(sid)"></el-badge>
              </span>
              <uci-form-item v-for="o in tab.options" :key="o.name" :option="o" :sid="sid"></uci-form-item>
            </el-tab-pane>
          </template>
        </el-tabs>
        <uci-form-item v-for="o in noTabOptions" :key="o.name" :option="o" :sid="sid"></uci-form-item>
      </div>
      <uci-section-del :sestion="sestion" :sid="sid"></uci-section-del>
    </div>
    <el-divider v-if="divider"></el-divider>
  </div>
</template>

<script>
import UciFormItem from './uci-form-item'
import UciSectionDel from './uci-section-del'

export default {
  name: 'UciSectionUi',
  inject: ['uciForm'],
  props: {
    sestion: Object,
    sid: String,
    divider: Boolean
  },
  components: {
    UciFormItem,
    UciSectionDel
  },
  computed: {
    tabs() {
      return this.sestion.tabs;
    },
    noTabOptions() {
      return this.sestion.arrayedOptions.filter(o => !o.tabName);
    }
  }
}
</script>

<style lang="scss">
.oui-uci-section-ui {
  display: flex;
  .el-button {
    height: 28px;
  }
}
</style>
