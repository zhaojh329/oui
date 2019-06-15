<template>
  <el-tabs v-if="radios.length > 0" :value="radios[0].name">
    <el-tab-pane v-for="radio in radios" :key="radio.name" :name="radio.name" :label="radio.name">
      <uci-form config="wireless">
        <uci-section :name="radio.name">
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option type="switch" :label="$t('Disabled')" name="disabled"></uci-option>
            <uci-option type="list" :label="$t('Transmit Power')" name="txpower" :options="radio.txpowerlist"></uci-option>
          </uci-tab>
          <uci-tab :title="$t('Advanced Settings')" name="advanced">
            <uci-option type="list" :label="$t('Country Code')" name="country" :options="radio.countrylist" initial="00" required></uci-option>
            <uci-option type="input" :label="$t('Distance Optimization')" name="distance" rules="uinteger"></uci-option>
          </uci-tab>
        </uci-section>
        <uci-section :title="$t('Interface')" type="wifi-iface" :option="{radio: radio.name}" :filter="filterInterface" addable>
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option type="switch" :label="$t('Disabled')" name="disabled"></uci-option>
            <uci-option type="list" :label="$t('Mode')" name="mode" required :options="modes"></uci-option>
            <uci-option type="input" label="SSID" name="ssid" required></uci-option>
            <uci-option type="list" :label="$t('Network')" name="network" :options="interfaces"></uci-option>
            <uci-option type="switch" :label="$t('Hide ESSID')" name="hidden" depend="mode == 'ap'"></uci-option>
            <uci-option type="switch" :label="$t('WMM Mode')" name="wmm" depend="mode == 'ap'" initial="1"></uci-option>
          </uci-tab>
          <uci-tab :title="$t('Wireless Security')" name="security">
            <uci-option type="list" :label="$t('Encryption')" name="encryption" :options="encryptions" initial="none"></uci-option>
            <uci-option type="input" :label="$t('Passphrase')" name="key" depend="encryption != 'none'" password></uci-option>
          </uci-tab>
          <uci-tab :title="$t('MAC-Filter')" name="macfilter">
            <uci-option type="list" :label="$t('Mode')" name="macfilter" :options="macfilters" depend="mode == 'ap'"></uci-option>
            <uci-option type="dlist" :label="$t('MAC-List')" name="maclist" depend="macfilter == 'allow' || macfilter == 'deny'" rules="macaddr"></uci-option>
          </uci-tab>
        </uci-section>
      </uci-form>
    </el-tab-pane>
  </el-tabs>
</template>

<script>
export default {
  data() {
    return {
      radios: [],
      modes: [
        ['ap', this.$t('Access Point')],
        ['sta', this.$t('Client')],
        ['adhoc', this.$t('Ad-Hoc')]
      ],
      interfaces: [],
      encryptions: [
        ['none', this.$t('No encryption')],
        ['psk', 'WPA-PSK'],
        ['psk2', 'WPA2-PSK'],
        ['psk-mixed', 'WPA/WPA2-PSK ' + this.$t('mixed')]
      ],
      macfilters: [
        ['allow', this.$t('Allow listed only')],
        ['deny', this.$t('Allow all except listed')]
      ]
    }
  },
  methods: {
    filterInterface(vm, s) {
      return vm.option.radio === s.device;
    }
  },
  created() {
    this.$uci.load('wireless').then(() => {
      const sections = this.$uci.sections('wireless', 'wifi-device');
      sections.forEach(s => {
        const device = s['.name'];
        const batch = [];

        batch.push(['iwinfo', 'freqlist', {device}]);
        batch.push(['iwinfo', 'txpowerlist', {device}]);
        batch.push(['iwinfo', 'countrylist', {device}]);

        this.$ubus.callBatch(batch).then(rs => {
          const txpowerlist = [];
          const countrylist = [];

          rs[1].results.forEach(tx => {
            txpowerlist.push([tx.dbm + '', `${tx.dbm} dBm (${tx.mw} mW)`]);
          });

          rs[2].results.forEach(c => {
            countrylist.push([c.code, `${c.code} - ${c.country}`]);
          });

          this.radios.push({
            name: device,
            freqlist: rs[0].results,
            txpowerlist: txpowerlist,
            countrylist: countrylist
          });
        });
      });
    });

    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces();
      this.interfaces = interfaces.map(item => item.name).filter(name => name !== 'loopback');
    });
  }
}
</script>
