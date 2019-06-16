<template>
  <div class="none-table">
    <uci-section-del :sestion="sestion" :sid="sid"></uci-section-del>
    <el-tabs v-if="tabs.length > 0" :value="tabs[0].name">
      <template v-for="tab in tabs">
        <el-tab-pane v-if="tab.options.length > 0" :key="tab.name" :name="tab.name">
          <span slot="label">
            {{ tab.title }}<el-badge v-if="tab.getErrorNum(sid) > 0" :value="tab.getErrorNum(sid)"></el-badge>
          </span>
          <uci-form-item v-for="o in tab.options" :key="o.name" :option="o" :sid="sid" :form="form"></uci-form-item>
        </el-tab-pane>
      </template>
    </el-tabs>
    <uci-form-item v-for="o in noTabOptions" :key="o.name" :option="o" :sid="sid" :form="form"></uci-form-item>
    <el-divider v-if="divider"></el-divider>
  </div>
</template>

<script>
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
    UciSectionDel
  },
  computed: {
    form() {
      return this.uciForm.form;
    },
    tabs() {
      return this.sestion.tabs;
    },
    noTabOptions() {
      return this.sestion.arrayedOptions.filter(o => !o.tabName);
    }
  }
}
</script>
