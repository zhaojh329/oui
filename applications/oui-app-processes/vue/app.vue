<template>
  <a-table :columns="columns" :data-source="data" :loading="loading">
    <template #signal="record">
      <a-button type="primary" size="small" @click="kill(record.pid, 1)" style="margin-right: 5px">{{ $t('processes.Hang Up') }}</a-button>
      <a-button type="danger" size="small" @click="kill(record.pid, 15)" style="margin-right: 5px">{{ $t('processes.Terminate') }}</a-button>
      <a-button type="danger" size="small" @click="kill(record.pid, 9)">{{ $t('processes.Kill immediately') }}</a-button>
    </template>
  </a-table>
</template>

<script>

export default {
  data () {
    return {
      columns: [
        { dataIndex: 'pid', title: 'PID', width: 100, sorter: (a, b) => a.pid - b.pid, defaultSortOrder: 'ascend' },
        { dataIndex: 'user', title: this.$t('processes.Owner'), width: 100 },
        { dataIndex: 'command', title: this.$t('processes.Command') },
        { dataIndex: 'cpu_percent', title: this.$t('processes.CPU usage'), width: 130, customRender: text => text + '%', sorter: (a, b) => a.cpu_percent - b.cpu_percent },
        { dataIndex: 'vsize_percent', title: this.$t('processes.Memory usage'), width: 160, customRender: text => text + '%', sorter: (a, b) => a.vsize_percent - b.vsize_percent },
        { key: 'signal', title: this.$t('processes.Signal'), scopedSlots: { customRender: 'signal' }, width: 240 }
      ],
      data: [],
      loading: true
    }
  },
  methods: {
    kill (pid, signum) {
      this.$rpc.ubus('system', 'signal', { pid, signum }).then(() => {
        this.$message.success(this.$t('Send signal to', { signum, pid }), 1)
      })
    }
  },
  created () {
    this.$rpc.call('system', 'process_list').then(({ processes }) => {
      this.data = processes.map((p, i) => {
        p.key = i
        return p
      })
      this.loading = false
    })
  }
}
</script>
