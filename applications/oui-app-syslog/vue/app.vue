<template>
  <a-table :columns="columns" :data-source="dataSource" :loading="loading">
    <template #msgTitle>
      <a-input :addon-before="$t('syslog.Message')" :placeholder="$t('syslog.Enter keywords to filter')" v-model="search" allow-clear/>
    </template>
  </a-table>
</template>

<script>
export default {
  data () {
    return {
      columns: [
        { dataIndex: 'datetime', title: this.$t('syslog.Datetime'), width: 220 },
        { dataIndex: 'facility', title: this.$t('syslog.Facility'), width: 100 },
        { dataIndex: 'level', title: this.$t('syslog.Level'), width: 100 },
        { dataIndex: 'msg', slots: { title: 'msgTitle' } }
      ],
      loading: true,
      syslog: [],
      search: ''
    }
  },
  computed: {
    dataSource () {
      return this.syslog.filter(log => !this.search || log.msg.includes(this.search))
    }
  },
  created () {
    this.$rpc.call('system', 'syslog').then(({ log }) => {
      this.syslog = log.map((v, i) => {
        v.key = i
        return v
      })
      this.loading = false
    })
  }
}
</script>
