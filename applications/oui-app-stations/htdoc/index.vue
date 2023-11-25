<template>
  <el-table :data="stations">
    <el-table-column prop="band" :label="$t('Type')" width="80"/>
    <el-table-column prop="ifname" :label="$t('Network')" width="100"/>
    <el-table-column :label="$t('MAC address')" width="150">
      <template #default="{ row }">
        <span>{{ row.macaddr.toUpperCase() }}</span>
      </template>
    </el-table-column>
    <el-table-column :label="$t('Signal / Noise')" width="220">
      <template #default="{ row }">
        <span>{{ `${row.signal} dBm / ${row.noise} dBm (SNR ${row.signal - row.noise})` }}</span>
      </template>
    </el-table-column>
    <el-table-column :label="$t('RX Rate / TX Rate')">
      <template #default="{ row }">
        <div>
          <p>{{ wifiRate(row.rx_rate) }}</p>
          <p>{{ wifiRate(row.tx_rate) }}</p>
        </div>
      </template>
    </el-table-column>
  </el-table>
</template>

<script>
export default {
  data() {
    return {
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
