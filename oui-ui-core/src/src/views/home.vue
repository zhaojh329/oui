<template>
  <div>
    <CardList :title="$t('System')" :list="sysinfo"></CardList>
    <CardList :title="$t('Network')" :list="waninfo"></CardList>
  </div>
</template>

<script>

export default {
  data() {
    return {
      sysinfo: [],
      waninfo: []
    }
  },
  methods: {
    formatLoad(v) {
      return parseFloat((v / 65535).toFixed(2));
    }
  },
  created() {
    this.$system.getInfo().then(r => {
      const load = r.load
      this.sysinfo = [
        [this.$t('Hostname'), r.hostname],
        [this.$t('Model'), r.model],
        [this.$t('Architecture'), r.system],
        [this.$t('Firmware Version'), r.release.revision],
        [this.$t('Kernel Version'), r.kernel],
        [this.$t('Local Time'), new Date(r.localtime * 1000).toString()],
        [this.$t('Uptime'), r.uptime],
        [this.$t('Load Average'), `${this.formatLoad(load[0])} ${this.formatLoad(load[1])} ${this.formatLoad(load[2])}`]
      ];
    });

    this.$network.load().then(() => {
      const iface = this.$network.getInterface('wan');
      this.waninfo = [
        [this.$t('IP Address'), iface.getIPv4Addrs().join(',')],
        [this.$t('Gateway'), iface.getIPv4Gateway()],
        ['DNS', iface.getDNSAddrs().join(',')]
      ];
    });
  }
}
</script>
