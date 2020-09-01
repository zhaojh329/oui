<template>
  <a-table :columns="columns" :data-source="initscripts">
    <template #start="text, record">
      <a-button :type="record.enabled ? 'success' : ''" size="small" @click="EnableDisable(record)" style="margin-right: 20px">{{ record.enabled ? $t('Enabled') : $t('Disabled') }}</a-button>
      <a-button type="primary" size="small" @click="action(record, 'start')" style="margin-right: 10px">{{ $t('Start') }}</a-button>
      <a-button type="danger" size="small" @click="action(record, 'restart')" style="margin-right: 10px">{{ $t('Restart') }}</a-button>
      <a-button type="danger" size="small" @click="action(record, 'stop')">{{ $t('Stop') }}</a-button>
    </template>
  </a-table>
</template>

<script>
export default {
  name: 'startup',
  data () {
    return {
      columns: [
        { dataIndex: 'start', title: this.$t('Start priority'), sorter: (a, b) => a.start - b.start, defaultSortOrder: 'ascend', width: 150 },
        { dataIndex: 'name', title: this.$t('Initscript') },
        { key: 'action', width: 280, scopedSlots: { customRender: 'start' } }
      ],
      initscripts: []
    }
  },
  methods: {
    load () {
      this.$system.initList().then(initscripts => {
        this.initscripts = initscripts.filter(item => typeof (item.start) !== 'undefined').map((v, i) => {
          v.key = i
          return v
        })
      })
    },
    EnableDisable (init) {
      if (init.enabled) {
        this.$system.initDisable(init.name)
      } else {
        this.$system.initEnable(init.name)
      }

      setTimeout(() => {
        this.load()
      }, 1000)
    },
    action (init, op) {
      if (op === 'start') {
        this.$system.initStart(init.name).then(this.load)
      } else if (op === 'restart') {
        this.$system.initRestart(init.name).then(this.load)
      } else {
        this.$system.initStop(init.name).then(this.load)
      }
    }
  },
  created () {
    this.load()
  }
}
</script>
