<template>
  <div>
    <n-card title="防火墙" style="margin-bottom: 16px">
      <n-tabs
        type="line"
        animated
        size="large"
        :value="activeTab"
        @update:value="handleUpdateValue"
      >
        <n-tab-pane
          :name="item.name"
          :tab="item.tab"
          v-for="item in tabOptions"
          :key="item.name"
          >{{ item.desc }}
        </n-tab-pane>
      </n-tabs>
    </n-card>
    <defaultSet v-show="activeTab === 'default'"></defaultSet>
    <ruleSet v-show="activeTab === 'rule'"></ruleSet>
  </div>
</template>

<script>
import defaultSet from './component/default.vue'
import ruleSet from './component/rule.vue'
export default {
  components: { defaultSet, ruleSet },
  data() {
    return {
      activeTab: 'default',
      tabOptions: [
        {
          tab: '常规设置',
          name: 'default',
          desc: '防火墙通过在网络接口上创建区域来控制网络流量。'
        },
        {
          tab: '通信规则',
          name: 'rule',
          desc: '通信规则定义了不同区域间的数据包传输策略，例如：拒绝一些主机之间的通信，开放路由器WAN 上的端口。'
        }
      ]
    }
  },
  methods: {
    handleUpdateValue(val) {
      this.activeTab = val
    }
  }
}
</script>

<style scoped></style>
