<template>
  <div>
    <card-table title="ARP" :headers="arp.headers" :items="arp.items"></card-table>
    <card-table title="Active IPv4-Routes" :headers="routes.headers" :items="routes.items"></card-table>
    <card-table title="Active IPv6-Routes" :headers="routes6.headers" :items="routes6.items"></card-table>
  </div>
</template>

<script>
  export default {
    data: () => ({
      arp: {
        headers: [
          {text: 'IPv4-Address', value: 'ipaddr'},
          {text: 'MAC-Address', value: 'macaddr'},
          {text: 'Interface', value: 'device'},
        ],
        items: []
      },
      routes: {
        headers: [
          {text: 'Target', value: 'target'},
          {text: 'Gateway', value: 'nexthop'},
          {text: 'Metric', value: 'metric'},
          {text: 'Interface', value: 'device'},
        ],
        items: []
      },
      routes6: {
        headers: [
          {text: 'Target', value: 'target'},
          {text: 'Source', value: 'source'},
          {text: 'Gateway', value: 'nexthop'},
          {text: 'Metric', value: 'metric'},
          {text: 'Interface', value: 'device'},
        ],
        items: []
      }
    }),
    mounted() {
      this.$ubus.call('vuci.network', 'arp_table').then(r => {
        this.arp.items = r.entries;
      });

      this.$ubus.call('vuci.network', 'routes').then(r => {
        this.routes.items = r.routes;
      });

      this.$ubus.call('vuci.network', 'routes6').then(r => {
        this.routes6.items = r.routes;
      });
    }
  };
</script>
