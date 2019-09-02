<template>
  <el-tabs v-if="radios.length > 0" :value="radios[0].name">
    <el-tab-pane v-for="radio in radios" :key="radio.name" :name="radio.name" :label="radio.name + ` (${radio.hardware})`">
      <uci-form config="wireless" :apply-timeout="15">
        <uci-section :name="radio.name">
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option-switch :label="$t('Disabled')" name="disabled"></uci-option-switch>
            <uci-option-list :label="$t('Mode')" name="hwmode" :options="radio.hwmodes" required></uci-option-list>
            <uci-option-list :label="$t('Band')" name="htmode" :options="radio.htmodes"></uci-option-list>
            <uci-option-list :label="$t('Channel')" name="channel" :options="radio.channels" :initial="radio.channel" required></uci-option-list>
            <uci-option-list :label="$t('Transmit Power')" name="txpower" :options="radio.txpowerlist" :initial="radio.txpower" required></uci-option-list>
          </uci-tab>
          <uci-tab :title="$t('Advanced Settings')" name="advanced">
            <uci-option-list :label="$t('Country Code')" name="country" :options="radio.countrylist" :initial="radio.country" required></uci-option-list>
            <uci-option-input :label="$t('Distance Optimization')" name="distance" rules="uinteger"></uci-option-input>
          </uci-tab>
        </uci-section>
        <uci-section :title="$t('Interface')" type="wifi-iface" :options="{radio: radio.name}" :filter="filterInterface" addable :after-add="afterAddIface">
          <uci-tab :title="$t('General Settings')" name="general">
            <uci-option-switch :label="$t('Disabled')" name="disabled"></uci-option-switch>
            <uci-option-list :label="$t('Mode')" name="mode" required :options="modes"></uci-option-list>
            <uci-option-input label="SSID" name="ssid" required></uci-option-input>
            <uci-option-list :label="$t('Network')" name="network" :options="interfaces"></uci-option-list>
            <uci-option-switch :label="$t('Hide ESSID')" name="hidden" depend="mode == 'ap'"></uci-option-switch>
            <uci-option-switch :label="$t('WMM Mode')" name="wmm" depend="mode == 'ap'" initial="1"></uci-option-switch>
          </uci-tab>
          <uci-tab :title="$t('Wireless Security')" name="security">
            <uci-option-list :label="$t('Encryption')" name="encryption" :options="encryptions" initial="none" :load="loadEncr" :save="saveEncr"></uci-option-list>
            <uci-option-list :label="$t('Cipher')" name="cipher" depend="encryption != 'none'" :options="ciphers" initial="auto" :load="loadCipher" :save="saveCipher"></uci-option-list>
            <uci-option-input :label="$t('Passphrase')" name="key" depend="encryption != 'none'" password></uci-option-input>
          </uci-tab>
          <uci-tab :title="$t('MAC-Filter')" name="macfilter">
            <uci-option-list :label="$t('Mode')" name="macfilter" :options="macfilters" depend="mode == 'ap'"></uci-option-list>
            <uci-option-dlist :label="$t('MAC-List')" name="maclist" depend="macfilter == 'allow' || macfilter == 'deny'" rules="macaddr"></uci-option-dlist>
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
      ciphers: [
        ['auto', this.$t('auto')],
        ['ccmp', this.$t('Force CCMP (AES)')],
        ['tkip', this.$t('Force TKIP')],
        ['tkip+ccmp', this.$t('Force TKIP and CCMP (AES)')]
      ],
      macfilters: [
        ['allow', this.$t('Allow listed only')],
        ['deny', this.$t('Allow all except listed')]
      ]
    }
  },
  methods: {
    filterInterface(s, self) {
      return self.options.radio === s.device;
    },
    afterAddIface(sid, self) {
      this.$uci.set('wireless', sid, 'device', self.options.radio);
    },
    loadEncr(sid) {
      const [v] = (this.$uci.get('wireless', sid, 'encryption') || '').split('+');
      return v;
    },
    loadCipher(sid) {
      let v = (this.$uci.get('wireless', sid, 'encryption') || '').split('+');

      if (v.length < 2)
        return 'auto';

      v = v.slice(1).join('+');

      if (v === 'aes')
        v = 'ccmp';
      else if (v === 'tkip+aes' || v === 'aes+tkip' || v === 'ccmp+tkip')
        v = 'tkip+ccmp';

      return v;
    },
    saveEncr(sid, value, self) {
      let cipher = self.uciSection.formValue('cipher', sid);

      if (cipher === 'tkip' || cipher === 'ccmp' || cipher === 'tkip+ccmp')
        value = `${value}+${cipher}`;

      this.$uci.set('wireless', sid, 'encryption', value);
    },
    saveCipher(sid, value, self) {
      let encr = self.uciSection.formValue('encryption', sid);

      if (value === 'tkip' || value === 'ccmp' || value === 'tkip+ccmp')
        encr = `${encr}+${value}`;

      this.$uci.set('wireless', sid, 'encryption', encr);
    }
  },
  created() {
    const loading = this.$getLoading();

    this.$uci.load('wireless').then(() => {
      const sections = this.$uci.sections('wireless', 'wifi-device');
      let radios_num = sections.length;

      sections.forEach(s => {
        const device = s['.name'];
        const batch = [];

        batch.push(['iwinfo', 'info', {device}]);
        batch.push(['iwinfo', 'freqlist', {device}]);
        batch.push(['iwinfo', 'txpowerlist', {device}]);
        batch.push(['iwinfo', 'countrylist', {device}]);

        this.$ubus.callBatch(batch).then(rs => {
          const channels = [['auto', this.$t('Automatic')]];
          const info = rs[0];
          const freqlist = rs[1].results
          const txpowerlist = [];
          const countrylist = [];

          freqlist.forEach(f => {
            if (f.restricted)
              return;
            channels.push([f.channel, `${f.channel} (${f.mhz} MHz)`]);
          });

          rs[2].results.forEach(tx => {
            txpowerlist.push([tx.dbm, `${tx.dbm} dBm (${tx.mw} mW)`]);
          });

          rs[3].results.forEach(c => {
            countrylist.push([c.code, `${c.code} - ${c.country}`]);
          });

          const hwmodes = ['11g'];

          if (info.hwmodes.indexOf('a') > -1 || info.hwmodes.indexOf('ac') > -1)
            hwmodes.push('11a');

          this.radios.push({
            name: device,
            channel: info.channel,
            txpower: info.txpower,
            country: info.country,
            hardware: info.hardware.name,
            hwmodes: hwmodes,
            htmodes: info.htmodes,
            channels: channels,
            txpowerlist: txpowerlist,
            countrylist: countrylist
          });

          radios_num--;

          if (radios_num === 0)
            loading.close();
        });
      });
    });

    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces();
      this.interfaces = interfaces.map(item => item.name);
    });
  }
}
</script>
