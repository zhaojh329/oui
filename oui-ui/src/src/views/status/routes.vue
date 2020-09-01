<template>
  <div>
    <a-table :title="() => 'ARP'" :columns="arp.columns" :data-source="arp.data" :loading="arp.loading"/>
    <a-table :title="() => $t('Active IPv4-Routes')" :columns="routes.columns" :data-source="routes.data" :loading="routes.loading"/>
    <a-table :title="() => $t('Active IPv6-Routes')" :columns="routes6.columns" :data-source="routes6.data" :loading="routes6.loading"/>
  </div>
</template>

<script>
export default {
  data () {
    return {
      arp: {
        columns: [
          { dataIndex: 'ipaddr', title: this.$t('IPv4-Address') },
          { dataIndex: 'macaddr', title: this.$t('MAC-Address') },
          { dataIndex: 'device', title: this.$t('Device') }
        ],
        data: [],
        loading: true
      },
      routes: {
        columns: [
          { dataIndex: 'target', title: this.$t('Target') },
          { dataIndex: 'nexthop', title: this.$t('Nexthop') },
          { dataIndex: 'metric', title: this.$t('Metric') },
          { dataIndex: 'device', title: this.$t('Device') }
        ],
        data: [],
        loading: true
      },
      routes6: {
        columns: [
          { dataIndex: 'target', title: this.$t('Target') },
          { dataIndex: 'source', title: this.$t('Source') },
          { dataIndex: 'nexthop', title: this.$t('Nexthop') },
          { dataIndex: 'metric', title: this.$t('Metric') },
          { dataIndex: 'device', title: this.$t('Device') }
        ],
        data: [],
        loading: true
      }
    }
  },
  created () {
    this.$rpc.call('network', 'arp_table').then(({ entries }) => {
      this.arp.data = entries.map((v, i) => {
        v.key = i
        return v
      })
      this.arp.loading = false
    })

    this.$rpc.call('network', 'routes').then(({ routes }) => {
      this.routes.data = routes.map((v, i) => {
        v.key = i
        return v
      })
      this.routes.loading = false
    })

    this.$rpc.call('network', 'routes6').then(({ routes }) => {
      this.routes6.data = routes.map((v, i) => {
        v.key = i
        return v
      })
      this.routes6.loading = false
    })
  }
}
</script>
