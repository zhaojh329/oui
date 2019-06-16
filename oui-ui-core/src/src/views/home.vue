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
    this.$system.getInfo().then(({hostname, load, model, system, release, kernel, localtime, uptime}) => {
      this.sysinfo = [
        [this.$t('Hostname'), hostname],
        [this.$t('Model'), model],
        [this.$t('Architecture'), system],
        [this.$t('Firmware Version'), release.revision],
        [this.$t('Kernel Version'), kernel],
        [this.$t('Local Time'), new Date(localtime * 1000).toString()],
        [this.$t('Uptime'), '%t'.format(uptime)],
        [this.$t('Load Average'), '%.2f %.2f %.2f'.format(load[0] / 65535, load[1] / 65535, load[2] / 65535)]
      ];
    });

    this.$network.load().then(() => {
      const iface = this.$network.getInterface('wan');
      this.waninfo = [
        [this.$t('IP Address'), iface.getIPv4Addrs().join(', ')],
        [this.$t('Gateway'), iface.getIPv4Gateway()],
        ['DNS', iface.getDNSAddrs().join(', ')]
      ];
    });
  }
}
</script>
