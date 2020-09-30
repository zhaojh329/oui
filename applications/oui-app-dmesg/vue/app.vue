<template>
  <a-table ref="dmesg-table" :columns="columns" :data-source="dataSource" :loading="loading">
    <template #msgTitle>
      <a-input :addon-before="$t('dmesg.Message')" :placeholder="$t('dmesg.Enter keywords to filter')" v-model="search" allow-clear/>
    </template>
  </a-table>
</template>

<script>
export default {
  data () {
    return {
      loading: true,
      columns: [
        { dataIndex: 'time', title: this.$t('dmesg.Time'), width: 150, sorter: true },
        { dataIndex: 'msg', slots: { title: 'msgTitle' } }
      ],
      dmesg: [],
      search: ''
    }
  },
  computed: {
    dataSource () {
      return this.dmesg.filter(log => !this.search || log.msg.includes(this.search))
    }
  },
  created () {
    this.$rpc.call('system', 'dmesg').then(({ log }) => {
      this.dmesg = log.map((v, i) => {
        v.key = i
        return v
      })
      this.loading = false
    })
  }
}
</script>
