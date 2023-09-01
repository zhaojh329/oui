<template>
  <n-data-table :row-key="r => r.macaddr" :columns="columns" :data="stations"/>
</template>

<script>
import { h } from 'vue'

export default {
  name: 'dhcp',
  data() {
    return {
      columns: [
        {
          title: () => this.$t('Type'),
          key: 'band'
        },
        {
          title: () => this.$t('Network'),
          key: 'ifname'
        },
        {
          title: () => this.$t('MAC address'),
          key: 'macaddr',
          render: r => r.macaddr.toUpperCase()
        },
        {
          title: () => this.$t('Signal'),
          key: 'signal',
          render: r => `${r.signal} dBm`
        },
        {
          title: () => this.$t('RX Rate / TX Rate'),
          key: 'rate',
          render: r => h('div', [
            h('p', this.wifiRate(r.rx_rate)),
            h('p', this.wifiRate(r.tx_rate))
          ])
        }
      ],
      stations: []
    }
  },
  methods: {
    wifiRate(rate) {
      let s = rate.rate + ' Mbit/s' + ', ' + rate.width + ' MHz'

      if (rate.eht) {
        s += ', EHT-MCS ' + rate.mcs
        s += ', EHT-NSS ' + rate.nss
        s += ', EHT-GI ' + rate.he_gi
      } else if (rate.he) {
        s += ', HE-MCS ' + rate.mcs
        s += ', HE-NSS ' + rate.nss
        s += ', HE-GI ' + rate.he_gi
        s += ', HE-DCM ' + rate.he_dcm
      } else if (rate.vht) {
        s += ', VHT-MCS ' + rate.mcs
        if (rate.nss)
          s += ', HE-NSS ' + rate.nss
      } else if (rate.mcs) {
        s += ', MCS ' + rate.mcs
      }

      if (rate.short_gi)
        s += ', short GI'

      return s
    },
    getStations() {
      this.$oui.call('station', 'stations').then(({ stations }) => {
        this.stations = stations
      })
    }
  },
  created() {
    this.$timer.create('getStations', this.getStations, { time: 3000, immediate: true, repeat: true })
  }
}
</script>

<i18n src="./locale.json"/>
