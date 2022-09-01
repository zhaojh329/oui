<template>
  <div>
    <n-card title="常规设置" style="margin-bottom: 16px">
      <n-grid x-gap="12" :cols="4" style="margin-bottom: 16px">
        <n-gi>启用SYN-flood防御</n-gi>
        <n-gi>
          <n-switch
            v-model:value="syn_flood"
            checked-value="1"
            unchecked-value="0"
            @update:value="handleUpdateValue('syn_flood')"
          />
        </n-gi>
      </n-grid>
      <n-grid x-gap="12" :cols="4" style="margin-bottom: 16px">
        <n-gi>启用FullCone-NAT</n-gi>
        <n-gi>
          <n-switch
            v-model:value="fullcone"
            checked-value="1"
            unchecked-value="0"
            @update:value="handleUpdateValue('fullcone')"
          />
        </n-gi>
      </n-grid>
      <n-grid x-gap="12" :cols="4" style="margin-bottom: 16px">
        <n-gi>入站数据</n-gi>
        <n-gi>
          <n-select
            v-model:value="input"
            :options="targetOption"
            @update:value="handleUpdateValue('input')"
          />
        </n-gi>
      </n-grid>
      <n-grid x-gap="12" :cols="4" style="margin-bottom: 16px">
        <n-gi>出站数据</n-gi>
        <n-gi>
          <n-select
            v-model:value="output"
            :options="targetOption"
            @update:value="handleUpdateValue('output')"
          />
        </n-gi>
      </n-grid>
      <n-grid x-gap="12" :cols="4">
        <n-gi>转发</n-gi>
        <n-gi>
          <n-select
            v-model:value="forward"
            :options="targetOption"
            @update:value="handleUpdateValue('forward')"
          />
        </n-gi>
      </n-grid>
    </n-card>
  </div>
</template>

<script>
import { targetOption } from '../dict'
export default {
  data() {
    return {
      syn_flood: '1',
      fullcone: '0',
      input: 'ACCEPT',
      output: 'ACCEPT',
      forward: 'ACCEPT',
      targetOption,
      keyList: ['syn_flood', 'fullcone', 'input', 'output', 'forward']
    }
  },
  async mounted() {
    for (let i = 0; i < this.keyList.length; i++) {
      await this.getDeviceConfig(this.keyList[i])
    }
  },
  methods: {
    async getDeviceConfig(flag) {
      this[flag] = await this.$oui.call('uci', 'get', {
        config: 'firewall',
        section: '@defaults[0]',
        option: flag
      })
    },
    async handleUpdateValue(flag) {
      await this.$oui.call('uci', 'set', {
        config: 'firewall',
        section: '@defaults[0]',
        values: { [flag]: this[flag] }
      })
      await this.getDeviceConfig(flag)
      this.$message.success('修改成功')
    }
  }
}
</script>
