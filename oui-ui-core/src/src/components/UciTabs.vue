<template>
  <el-tabs :value="tabpanes[0][0]">
    <el-tab-pane v-for="item in tabpanes" :key="item[0]" :name="item[0]">
      <span slot="label">
        {{ item[1] }}<uci-badge :value="validated | tabErrorNum(item[0])"></uci-badge>
      </span>
      <slot :name="item[0]"></slot>
    </el-tab-pane>
  </el-tabs>
</template>

<script>
import UciBadge from './UciBadge.vue'

export default {
  name: 'UciTabs',
  props: {
    tabpanes: Array
  },
  components: {
    'UciBadge': UciBadge
  },
  filters: {
    tabErrorNum: function(validated, tab) {
      let num = 0;

      Object.keys(validated).forEach(key => {
        if (validated[key].tab === tab && !validated[key].valid)
          num++;
      });

      return num;
    }
  },
  computed: {
    validated() {
      return this.$getParent('UciForm').validated;
    }
  }
}
</script>

