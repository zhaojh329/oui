<template>
  <div>
    <a-table :title="() => 'ARP'" :columns="arp.columns" :data-source="arp.data" :loading="loading"/>
    <a-table :title="() => $t('Active IPv4-Routes')" :columns="routes.columns" :data-source="routes.data" :loading="loading"/>
    <a-table :title="() => $t('Active IPv6-Routes')" :columns="routes6.columns" :data-source="routes6.data" :loading="loading"/>
  </div>
</template>

<script>
export default {
  data () {
    return {
      loading: true,
      arp: {
        columns: [
          { dataIndex: 'ipaddr', title: this.$t('IPv4-Address') },
          { dataIndex: 'macaddr', title: this.$t('MAC-Address') },
          { dataIndex: 'device', title: this.$t('Device') }
        ],
        data: []
      },
      routes: {
        columns: [
          { dataIndex: 'target', title: this.$t('Target') },
          { dataIndex: 'nexthop', title: this.$t('Nexthop') },
          { dataIndex: 'metric', title: this.$t('Metric') },
          { dataIndex: 'device', title: this.$t('Device') }
        ],
        data: []
      },
      routes6: {
        columns: [
          { dataIndex: 'target', title: this.$t('Target') },
          { dataIndex: 'source', title: this.$t('Source') },
          { dataIndex: 'nexthop', title: this.$t('Nexthop') },
          { dataIndex: 'metric', title: this.$t('Metric') },
          { dataIndex: 'device', title: this.$t('Device') }
        ],
        data: []
      }
    }
  },
  created () {
    this.$ubus.callBatch([
      ['oui.network', 'arp_table'],
      ['oui.network', 'routes'],
      ['oui.network', 'routes6']
    ]).then(r => {
      this.arp.data = r[0].entries.map((v, i) => {
        v.key = i
        return v
      })
      this.routes.data = r[1].routes.map((v, i) => {
        v.key = i
        return v
      })
      this.routes6.data = r[2].routes.map((v, i) => {
        v.key = i
        return v
      })
      this.loading = false
    })
  }
}
</script>
