<template>
  <div>
    <div style="position: relative; top: 70px">
      <oui-line :length="lineLength" :style="{ position: 'absolute', left: lineOffset }" active/>
      <oui-line :length="lineLength" :style="{ position: 'absolute', right: lineOffset }" active :disconnect="!wanIsUp"/>
    </div>
    <div class="oui-home-tabs">
      <div>
        <div @click="tab = 'devices'" style="display:inline;" class="tab-tilte">
          <img src="/icons/devices.png"/>
          <div :class="{active: tab === 'devices'}">{{ $t('home.Terminal devices') + `(${devices.length})` }}</div>
        </div>
        <div @click="tab = 'router'" style="display:inline;" class="tab-tilte">
          <img src="/icons/router.png"/>
          <div :class="{active: tab === 'router'}">{{ $t('System') }}</div>
        </div>
        <div @click="tab = 'internet'" style="display:inline;" class="tab-tilte">
          <img src="/icons/internet.png"/>
          <div :class="{active: tab === 'internet'}">WAN</div>
        </div>
      </div>
      <div style="clear: both">
        <div v-show="tab === 'devices'">
          <a-card :title="$t('home.Online devices')" style="margin-bottom: 20px">
            <a-table :columns="devicesColumns" :data-source="devices"/>
          </a-card>
          <a-card :title="$t('home.Active DHCP Leases')" style="margin-bottom: 20px">
            <a-table :columns="leasesColumns" :data-source="leases"/>
          </a-card>
          <a-card :title="$t('home.Active DHCPv6 Leases')" style="margin-bottom: 20px">
            <a-table :columns="leases6Columns" :data-source="leases6"/>
          </a-card>
          <a-card :title="$t('home.Associated Stations')" style="margin-bottom: 20px">
            <a-table :columns="assoclistColumns" :data-source="assoclist">
              <template #signal="record">
                <img :src="wifiSignalIcon(record)"/>
                <span>{{ ' ' + record.signal + '/' + record.noise + ' dBm' }}</span>
              </template>
              <template #rx="record">
                <span>{{ formatWifiRxRate(record) }}</span>
              </template>
              <template #tx="record">
                <span>{{ formatWifiTxRate(record) }}</span>
              </template>
            </a-table>
          </a-card>
        </div>
        <div v-show="tab === 'router'">
          <div style="display: flex; justify-content: space-around;">
            <oui-dashboard :percent="cpuPercentage" :label="$t('home.CPU usage')"/>
            <oui-dashboard :percent="memPercentage" :label="$t('home.Memory usage')"/>
            <oui-dashboard :percent="storagePercentage" :label="$t('home.Storage')"/>
          </div>
          <a-descriptions :title="$t('home.System information')" :column="1" bordered>
            <a-descriptions-item v-for="item in sysinfo" :key="item[0]" :label="item[0]">{{ item[1] }}</a-descriptions-item>
          </a-descriptions>
        </div>
        <div v-show="tab === 'internet'">
          <a-descriptions :title="$t('Network')" :column="1" bordered>
            <a-descriptions-item v-for="item in waninfo" :key="item[0]" :label="item[0]">{{ item[1] }}</a-descriptions-item>
          </a-descriptions>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data () {
    return {
      tab: 'router',
      statusLineLength: '26%',
      sysinfo: [],
      waninfo: [],
      wanIsUp: false,
      devicesColumns: [
        { dataIndex: 'hostname', title: this.$t('Hostname') },
        { dataIndex: 'ipaddr', title: this.$t('home.IPv4-Address') },
        { dataIndex: 'macaddr', title: this.$t('home.MAC-Address') },
        { dataIndex: 'rxrate', title: this.$t('home.RX Rate') },
        { dataIndex: 'txrate', title: this.$t('home.TX Rate') }
      ],
      devices: [],
      devicesMap: {},
      leasesColumns: [
        { dataIndex: 'hostname', title: this.$t('Hostname') },
        { dataIndex: 'ipaddr', title: this.$t('home.IPv4-Address') },
        { dataIndex: 'macaddr', title: this.$t('home.MAC-Address') },
        { dataIndex: 'leasetime', title: this.$t('home.Leasetime remaining'), customRender: text => this.leasetime(text) }
      ],
      leases: [],
      leases6Columns: [
        { dataIndex: 'name', title: this.$t('home.Host') },
        { dataIndex: 'ip6addr', title: this.$t('home.IPv6-Address') },
        { dataIndex: 'duid', title: 'DUID' },
        { dataIndex: 'leasetime', title: this.$t('home.Leasetime remaining'), customRender: text => this.leasetime(text) }
      ],
      leases6: [],
      assoclistColumns: [
        { dataIndex: 'mac', title: this.$t('home.MAC-Address') },
        { dataIndex: 'host', title: this.$t('home.Host') },
        { key: 'signal', title: this.$t('home.Signal') + ' / ' + this.$t('home.Noise'), scopedSlots: { customRender: 'signal' } },
        { key: 'rx', title: this.$t('home.RX Rate'), scopedSlots: { customRender: 'rx' } },
        { key: 'tx', title: this.$t('home.TX Rate'), scopedSlots: { customRender: 'tx' } }
      ],
      assoclist: [],
      lastCPUTime: null,
      cpuPercentage: 100,
      memPercentage: 100,
      storagePercentage: 100
    }
  },
  timers: {
    update: { time: 2000, autostart: true, immediate: true, repeat: true },
    updateCpuUsage: { time: 1000, autostart: true, immediate: true, repeat: true }
  },
  methods: {
    leasetime (expires) {
      return expires <= 0 ? this.$t('home.expired') : '%t'.format(expires)
    },
    wifirate (sta, rx) {
      const rate = rx ? sta.rx : sta.tx
      let s = '%.1f Mbit/s'.format(rate.rate / 1000)
      s += ', %dMHz'.format(rate['40mhz'] ? 40 : 20)

      if (rate.mcs > 0) { s += ', MCS ' + rate.mcs }

      if (rate.short_gi) { s += ', Short GI' }

      return s
    },
    wifiSignalIcon (s) {
      let q = (-1 * (s.noise - s.signal)) / 5
      if (q < 1) { q = 1 } else if (q < 2) { q = 2 } else if (q < 3) { q = 3 } else if (q < 4) { q = 4 } else { q = 5 }
      return `/icons/signal_${q}.png`
    },
    formatWifiRxRate (row) {
      return this.wifirate(row, true)
    },
    formatWifiTxRate (row) {
      return this.wifirate(row, false)
    },
    updateCpuUsage () {
      this.$rpc.call('system', 'cpu_time').then(times => {
        if (!this.lastCPUTime) {
          this.cpuPercentage = 0
          this.lastCPUTime = times
          return
        }

        const idle1 = this.lastCPUTime[3]
        const idle2 = times[3]

        let total1 = 0
        let total2 = 0

        this.lastCPUTime.forEach(t => {
          total1 += t
        })

        times.forEach(t => {
          total2 += t
        })

        this.cpuPercentage = Math.floor(((total2 - total1 - (idle2 - idle1)) / (total2 - total1)) * 100)
        this.lastCPUTime = times
      })
    },
    update () {
      this.$system.getInfo().then(({ hostname, model, system, release, kernel, localtime, uptime, memory }) => {
        this.sysinfo = [
          [this.$t('Hostname'), hostname],
          [this.$t('home.Model'), model],
          [this.$t('home.Architecture'), system],
          [this.$t('home.Firmware Version'), release.revision],
          [this.$t('home.Kernel Version'), kernel],
          [this.$t('home.Local Time'), new Date(localtime * 1000).toString()],
          [this.$t('Uptime'), '%t'.format(uptime)]
        ]

        this.memPercentage = Math.floor(((memory.total - memory.free) / memory.total) * 100)
      })

      this.$system.getDiskInfo().then(({ root }) => {
        this.storagePercentage = Math.floor((root.used / root.total) * 100)
      })

      this.$network.load().then(() => {
        const iface = this.$network.getInterface('wan')
        if (!iface) {
          this.waninfo = []
          this.wanIsUp = false
          return
        }

        this.waninfo = [
          [this.$t('home.IP Address'), iface.getIPv4Addrs().join(', ')],
          [this.$t('home.Gateway'), iface.getIPv4Gateway()],
          ['DNS', iface.getDNSAddrs().join(', ')]
        ]
        this.wanIsUp = iface.isUp()
      })

      this.$rpc.call('network', 'dhcp_leases').then(r => {
        const leasesMap = {}

        this.leases = r.leases.map(l => {
          l.key = l.ipaddr
          return l
        })

        this.leases.forEach(l => {
          leasesMap[l.macaddr] = { hostname: l.hostname, ipaddr: l.ipaddr }
        })

        this.$rpc.call('network', 'bwm').then(r => {
          this.devices = r.entries.map(dev => {
            const lease = leasesMap[dev.macaddr]

            dev = { ...dev, txrate: 0, rxrate: 0 }

            const ldev = this.devicesMap[dev.macaddr]
            if (ldev) {
              dev.txrate = '%mB/s'.format((dev.tx - ldev.tx) / 2)
              dev.rxrate = '%mB/s'.format((dev.rx - ldev.rx) / 2)
            }

            this.devicesMap[dev.macaddr] = { tx: dev.tx, rx: dev.rx }

            if (lease) dev.hostname = lease.hostname

            dev.key = dev.macaddr

            return dev
          })
        })

        this.$wireless.getAssoclist().then(assoclist => {
          this.assoclist = assoclist.map(sta => {
            const lease = leasesMap[sta.mac.toLowerCase()]
            if (lease) sta.host = `${lease.hostname} (${lease.ipaddr})`

            sta.key = sta.mac

            return sta
          })
        })
      })

      this.$rpc.call('network', 'dhcp6_leases').then(r => {
        this.leases6 = r.leases.map(l => {
          l.key = l.ipaddr
          return l
        })
      })
    }
  },
  computed: {
    lineLength () {
      return 'calc(100% / 3 - 160px)'
    },
    lineOffset () {
      return 'calc(100% / 6 + 66px)'
    }
  }
}
</script>

<style>
  .tab-tilte {
    float: left;
    width: calc(100% / 3);
    padding: 10px 0;
    text-align: center;
    cursor: pointer;
  }

  .tab-tilte div {
    background-color: #e8e8e8;
  }

  .tab-tilte:first-child div {
    border-radius: 20px 0 0 0;
  }

  .tab-tilte:last-child div {
    border-radius: 0 20px 0 0;
  }

  .tab-tilte .active {
    background-color: #09f;
    color: #fff;
  }
</style>
