<template>
  <div>
    <CardTable title="ARP" :columns="arp.columns" :data="arp.data"></CardTable>
    <CardTable title="Active IPv4-Routes" :columns="routes.columns" :data="routes.data"></CardTable>
    <CardTable title="Active IPv6-Routes" :columns="routes6.columns" :data="routes6.data"></CardTable>
  </div>
</template>

<script>

export default {
  data() {
    return {
      arp: {
        columns: [
          {key: 'ipaddr', title: 'IPv4-Address'},
          {key: 'macaddr', title: 'MAC-Address'},
          {key: 'device', title: 'Device'}
        ],
        data: []
      },
      routes: {
        columns: [
          {key: 'target', title: 'Target'},
          {key: 'nexthop', title: 'Nexthop'},
          {key: 'metric', title: 'Metric'},
          {key: 'device', title: 'Device'}
        ],
        data: []
      },
      routes6: {
        columns: [
          {key: 'target', title: 'Target'},
          {key: 'source', title: 'Source'},
          {key: 'nexthop', title: 'Nexthop'},
          {key: 'metric', title: 'Metric'},
          {key: 'device', title: 'Device'}
        ],
        data: []
      }
    }
  },
  mounted() {
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
