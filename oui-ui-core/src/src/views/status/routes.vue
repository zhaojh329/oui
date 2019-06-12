<template>
  <div>
    <CardTable title="ARP" :columns="arp.columns" :data="arp.data"></CardTable>
    <CardTable :title="$t('Active IPv4-Routes')" :columns="routes.columns" :data="routes.data"></CardTable>
    <CardTable :title="$t('Active IPv6-Routes')" :columns="routes6.columns" :data="routes6.data"></CardTable>
  </div>
</template>

<script>

export default {
  data() {
    return {
      arp: {
        columns: [
          {key: 'ipaddr', title: this.$t('IPv4-Address')},
          {key: 'macaddr', title: this.$t('MAC-Address')},
          {key: 'device', title: this.$t('Device')}
        ],
        data: []
      },
      routes: {
        columns: [
          {key: 'target', title: this.$t('Target')},
          {key: 'nexthop', title: this.$t('Nexthop')},
          {key: 'metric', title: this.$t('Metric')},
          {key: 'device', title: this.$t('Device')}
        ],
        data: []
      },
      routes6: {
        columns: [
          {key: 'target', title: this.$t('Target')},
          {key: 'source', title: this.$t('Source')},
          {key: 'nexthop', title: this.$t('Nexthop')},
          {key: 'metric', title: this.$t('Metric')},
          {key: 'device', title: this.$t('Device')}
        ],
        data: []
      }
    }
  },
  created() {
    this.$ubus.callBatch([
      ['oui.network', 'arp_table'],
      ['oui.network', 'routes'],
      ['oui.network', 'routes6']
    ]).then(r => {
      this.arp.data = r[0].entries;
      this.routes.data = r[1].routes;
      this.routes6.data = r[2].routes;
    });
  }
}
</script>
