<template>
  <oui-form uci-config="wireless">
    <a-tabs v-if="radios.length > 0" name="top" :animated="false">
      <a-tab-pane v-for="radio in radios" :key="radio.name" :tab="radio.name + ` (${radio.hardware})`">
        <oui-named-section :name="radio.name" v-slot="{ s }" :card="false">
          <a-tabs>
            <a-tab-pane :tab="$t('General Settings')" key="general">
              <oui-form-item-switch :uci-section="s" :label="$t('Disabled')" name="disabled"/>
              <oui-form-item-select :uci-section="s" :label="$t('Mode')" name="hwmode" :options="radio.hwmodes" required/>
              <oui-form-item-select :uci-section="s" :label="$t('Band')" name="htmode" :options="radio.htmodes"/>
              <oui-form-item-select :uci-section="s" :label="$t('Channel')" name="channel" :options="radio.channels" :initial="radio.channel" required :placeholder="$t('auto')"/>
              <oui-form-item-select :uci-section="s" :label="$t('Transmit Power')" name="txpower" :options="radio.txpowerlist" :initial="radio.txpower" required/>
            </a-tab-pane>
            <a-tab-pane :tab="$t('Advanced Settings')" key="advanced">
              <oui-form-item-select :uci-section="s" :label="$t('Country Code')" name="country" :options="radio.countrylist" :initial="radio.country" required/>
              <oui-form-item-input :uci-section="s" :label="$t('Distance Optimization')" name="distance" rules="uinteger"/>
            </a-tab-pane>
          </a-tabs>
        </oui-named-section>
        <oui-typed-section :title="$t('Interface')" type="wifi-iface" :filter="(s => s.device === radio.name)" addremove v-slot="{ s }"
                         :teasers="['disabled', 'mode', 'ssid', 'network', 'encryption']" :add="self => AddIface(self, radio.name)">
          <a-tabs>
            <a-tab-pane :tab="$t('General Settings')" key="general">
              <oui-form-item-switch :uci-section="s" :label="$t('Disabled')" name="disabled"/>
              <oui-form-item-select :uci-section="s" :label="$t('Mode')" name="mode" required :options="modes"/>
              <oui-form-item-input :uci-section="s" label="SSID" name="ssid" required/>
              <oui-form-item-select :uci-section="s" :label="$t('Network')" name="network" :options="interfaces"/>
              <oui-form-item-switch :uci-section="s" :label="$t('Hide ESSID')" name="hidden" depend="mode == 'ap'"/>
              <oui-form-item-switch :uci-section="s" :label="$t('WMM Mode')" name="wmm" depend="mode == 'ap'" initial/>
            </a-tab-pane>
            <a-tab-pane :tab="$t('Wireless Security')" key="security">
              <oui-form-item-select :uci-section="s" :label="$t('Encryption')" name="encryption" :options="encryptions" initial="none" :load="loadEncr" :save="saveEncr"/>
              <oui-form-item-select :uci-section="s" :label="$t('Cipher')" name="cipher" depend="encryption != 'none'" :options="ciphers" initial="auto" :load="loadCipher" :save="saveCipher"/>
              <oui-form-item-input :uci-section="s" :label="$t('Passphrase')" name="key" depend="encryption != 'none'" password/>
            </a-tab-pane>
            <a-tab-pane :tab="$t('MAC-Filter')" key="mac-filter">
              <oui-form-item-select :uci-section="s" :label="$t('Mode')" name="macfilter" :options="macfilters" depend="mode == 'ap'"/>
              <oui-form-item-list :uci-section="s" :label="$t('MAC-List')" name="maclist" depend="macfilter == 'allow' || macfilter == 'deny'" rules="macaddr"/>
            </a-tab-pane>
          </a-tabs>
        </oui-typed-section>
      </a-tab-pane>
    </a-tabs>
  </oui-form>
</template>

<script>
export default {
  data () {
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
    AddIface (self, radio) {
      const sid = self.addSection()
      self.set(sid, 'device', radio)
    },
    loadEncr (self) {
      const [v] = (this.$uci.get('wireless', self.sid, 'encryption') || '').split('+')
      return v
    },
    loadCipher (self) {
      let v = (this.$uci.get('network', self.sid, 'encryption') || '').split('+')

      if (v.length < 2) { return 'auto' }

      v = v.slice(1).join('+')

      if (v === 'aes') {
        v = 'ccmp'
      } else if (v === 'tkip+aes' || v === 'aes+tkip' || v === 'ccmp+tkip') {
        v = 'tkip+ccmp'
      }

      return v
    },
    saveEncr (self) {
      const cipher = this.$uci.get('network', self.sid, 'cipher')
      let value = self.model

      if (cipher === 'tkip' || cipher === 'ccmp' || cipher === 'tkip+ccmp') {
        value = `${value}+${cipher}`
      }

      this.$uci.set('network', self.sid, 'encryption', value)
    },
    saveCipher (self) {
      let encr = this.$uci.get(self.sid, 'encryption')
      const value = self.model

      if (value === 'tkip' || value === 'ccmp' || value === 'tkip+ccmp') {
        encr = `${encr}+${value}`
      }

      this.$uci.set('network', self.sid, 'encryption', encr)
    }
  },
  created () {
    this.$spin()

    this.$uci.load('wireless').then(() => {
      const sections = this.$uci.sections('wireless', 'wifi-device')
      let radiosNum = sections.length

      sections.forEach(s => {
        const device = s['.name']
        const batch = []

        batch.push(['iwinfo', 'info', { device }])
        batch.push(['iwinfo', 'freqlist', { device }])
        batch.push(['iwinfo', 'txpowerlist', { device }])
        batch.push(['iwinfo', 'countrylist', { device }])

        this.$ubus.callBatch(batch).then(rs => {
          const channels = [['auto', this.$t('Automatic')]]
          const info = rs[0]
          const freqlist = rs[1].results
          const txpowerlist = []
          const countrylist = []

          freqlist.forEach(f => {
            if (f.restricted) { return }
            channels.push([f.channel, `${f.channel} (${f.mhz} MHz)`])
          })

          rs[2].results.forEach(tx => {
            txpowerlist.push([tx.dbm, `${tx.dbm} dBm (${tx.mw} mW)`])
          })

          rs[3].results.forEach(c => {
            countrylist.push([c.code, `${c.code} - ${c.country}`])
          })

          const hwmodes = ['11g']

          if (info.hwmodes.indexOf('a') > -1 || info.hwmodes.indexOf('ac') > -1) { hwmodes.push('11a') }

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
          })

          radiosNum--

          if (radiosNum === 0) {
            this.$spin(false)
          }
        })
      })
    })

    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces()
      this.interfaces = interfaces.map(item => item.name)
    })
  }
}
</script>
