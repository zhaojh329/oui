<template>
  <div>
    <el-row type="flex" justify="center" align="middle" class="oui-home-status-img">
      <img src="/icons/devices.png" @click="tab = 'devices'" />
      <oui-line :length="statusLineLength"></oui-line>
      <img src="/icons/router.png" @click="tab = 'router'" />
      <oui-line :length="statusLineLength" :disconnect="!wanIsUp"></oui-line>
      <img src="/icons/internet.png" @click="tab = 'internet'" />
    </el-row>
    <el-tabs v-model="tab" stretch>
      <el-tab-pane name="devices">
        <span slot="label">{{ $t('Terminal devices') + `(${devices.length})` }}</span>
        <el-card :header="$t('Online devices')" style="margin-bottom: 15px;">
          <el-table :data="devices">
            <el-table-column :label="$t('Hostname')" prop="hostname"></el-table-column>
            <el-table-column :label="$t('IPv4-Address')" prop="ipaddr"></el-table-column>
            <el-table-column :label="$t('MAC-Address')" prop="macaddr"></el-table-column>
            <el-table-column :label="$t('RX Rate')" prop="rxrate"></el-table-column>
            <el-table-column :label="$t('TX Rate')" prop="txrate"></el-table-column>
          </el-table>
        </el-card>
        <el-card :header="$t('Active DHCP Leases')" style="margin-bottom: 15px;">
          <el-table :data="leases">
            <el-table-column :label="$t('Hostname')" prop="hostname"></el-table-column>
            <el-table-column :label="$t('IPv4-Address')" prop="ipaddr"></el-table-column>
            <el-table-column :label="$t('MAC-Address')" prop="macaddr"></el-table-column>
            <el-table-column :label="$t('Leasetime remaining')" :formatter="row => row.expires <= 0 ? $t('expired') : '%t'.format(row.expires)"></el-table-column>
          </el-table>
        </el-card>
        <el-card :header="$t('Active DHCPv6 Leases')" style="margin-bottom: 15px;">
          <el-table :data="leases6">
            <el-table-column :label="$t('Host')" prop="name"></el-table-column>
            <el-table-column :label="$t('IPv6-Address')" prop="ip6addr"></el-table-column>
            <el-table-column label="DUID" prop="duid"></el-table-column>
            <el-table-column :label="$t('Leasetime remaining')" :formatter="row => row.expires <= 0 ? $t('expired') : '%t'.format(row.expires)"></el-table-column>
          </el-table>
        </el-card>
        <el-card :header="$t('Associated Stations')">
          <el-table :data="assoclist">
            <el-table-column :label="$t('MAC-Address')" prop="mac"></el-table-column>
            <el-table-column :label="$t('Host')" prop="host"></el-table-column>
            <el-table-column :label="$t('Signal') + ' / ' + $t('Noise')">
              <template v-slot="{row}">
                <img :src="wifiSignalIcon(row)" />
                <span>{{' ' + row.signal + '/' + row.noise + ' dBm' }}</span>
              </template>
            </el-table-column>
            <el-table-column :label="$t('RX Rate')" :formatter="formatWifiRxRate"></el-table-column>
            <el-table-column :label="$t('TX Rate')" :formatter="formatWifiTxRate"></el-table-column>
          </el-table>
        </el-card>
      </el-tab-pane>
      <el-tab-pane name="router">
        <span slot="label">{{ $t('System') }}</span>
        <el-row :gutter="20" style="margin-bottom: 15px">
          <el-col :span="12">
            <card-list :title="$t('System information')" :list="sysinfo"></card-list>
          </el-col>
          <el-col :span="12">
            <el-card :header="$t('Resource usage')">
              <e-charts style="width: 100%" :options="resourceChart"></e-charts>
            </el-card>
          </el-col>
        </el-row>
      </el-tab-pane>
      <el-tab-pane name="internet">
        <span slot="label">WAN</span>
        <el-row type="flex" justify="center">
          <CardList :title="$t('Network')" :list="waninfo" style="width: 600px"></CardList>
        </el-row>
      </el-tab-pane>
    </el-tabs>
  </div>
</template>

<script>
import 'echarts/lib/chart/line'
import 'echarts/lib/chart/gauge'
import 'echarts/lib/component/title'
import 'echarts/lib/component/polar'
import 'echarts/lib/component/tooltip'
import 'echarts/lib/component/legendScroll'

export default {
  data() {
    return {
      tab: 'router',
      statusLineLength: '27%',
      sysinfo: [],
      waninfo: [],
      devices: [],
      devicesMap: {},
      leases: [],
      leases6: [],
      assoclist: [],
      wanIsUp: false,
      lastCPUTime: null,
      resourceChart: {
        tooltip: {
          formatter: '{b}: {c}%'
        },
        series: [
          {
            type: 'gauge',
            center: ['25%', '50%'],
            detail: {formatter: '{value}%'},
            data: [{value: 0, name: this.$t('CPU usage')}]
          },
          {
            type: 'gauge',
            center: ['75%', '50%'],
            detail: {formatter: '{value}%'},
            data: [{value: 0, name: this.$t('Memory usage')}]
          }
        ]
      }
    }
  },
  timers: {
    update: {time: 2000, autostart: true, immediate: true, repeat: true}
  },
  methods: {
    wifirate(sta, rx) {
      const rate = rx ? sta.rx : sta.tx;
      let s = '%.1f Mbit/s'.format(rate.rate / 1000);
      s += ', %dMHz'.format(rate['40mhz'] ? 40 : 20);

      if (rate.mcs > 0)
        s += ', MCS ' + rate.mcs;

      if (rate.short_gi)
        s += ', Short GI'

      return s;
    },
    wifiSignalIcon(s) {
      let q = (-1 * (s.noise - s.signal)) / 5;
      if (q < 1)
        q = 1;
      else if (q < 2)
        q = 2;
      else if (q < 3)
        q = 3;
      else if (q < 4)
        q = 4;
      else
        q = 5;
      return `/icons/signal_${q}.png`;
    },
    formatWifiRxRate(row) {
      return this.wifirate(row, true);
    },
    formatWifiTxRate(row) {
      return this.wifirate(row, false);
    },
    calcDevFlow(flow) {
      return flow[0] * 1000000000 + flow[1] * 1000000 + flow[2] * 1000 + flow[3];
    },
    update() {
      this.$ubus.call('oui.system', 'cpu_time').then(({times}) => {
        if (!this.lastCPUTime) {
          this.lastCPUTime = times;
          return;
        }

        let idle1 = this.lastCPUTime[3] + this.lastCPUTime[4];
        let idle2 = times[3] + times[4];

        let total1 = 0;
        let total2 = 0;

        this.lastCPUTime.forEach(t => {
          total1 += t;
        });

        times.forEach(t => {
          total2 += t;
        });

        this.resourceChart.series[0].data[0].value = (((total2 - total1) - (idle2 - idle1)) / (total2 - total1) * 100).toFixed(2)
        this.lastCPUTime = times;
      });

      this.$system.getInfo().then(({hostname, model, system, release, kernel, localtime, uptime, memory}) => {
        this.sysinfo = [
          [this.$t('Hostname'), hostname],
          [this.$t('Model'), model],
          [this.$t('Architecture'), system],
          [this.$t('Firmware Version'), release.revision],
          [this.$t('Kernel Version'), kernel],
          [this.$t('Local Time'), new Date(localtime * 1000).toString()],
          [this.$t('Uptime'), '%t'.format(uptime)]
        ];

        this.resourceChart.series[1].data[0].value = ((memory.total - memory.free) / memory.total * 100).toFixed(2);
      });

      this.$network.load().then(() => {
        const iface = this.$network.getInterface('wan');
        this.waninfo = [
          [this.$t('IP Address'), iface.getIPv4Addrs().join(', ')],
          [this.$t('Gateway'), iface.getIPv4Gateway()],
          ['DNS', iface.getDNSAddrs().join(', ')]
        ];
        this.wanIsUp = iface.isUp()
      });

      this.$ubus.call('oui.network', 'dhcp_leases').then(r => {
        const leasesMap = {};

        this.leases = r.leases;

        this.leases.forEach(l => {
          leasesMap[l.macaddr] = {hostname: l.hostname, ipaddr: l.ipaddr};
        });

        this.$ubus.call('oui.network', 'bwm').then(r => {
          this.devices = r.entries.map(dev => {
            const lease = leasesMap[dev.macaddr];

            dev = {...dev, txrate: 0, rxrate: 0};
            dev.tx = this.calcDevFlow(dev.tx);
            dev.rx = this.calcDevFlow(dev.rx);

            const ldev = this.devicesMap[dev.macaddr];
            if (ldev) {
              dev.txrate = '%mB/s'.format((dev.tx - ldev.tx) / 2);
              dev.rxrate = '%mB/s'.format((dev.rx - ldev.rx) / 2);
            }

            this.devicesMap[dev.macaddr] = {tx: dev.tx, rx: dev.rx};

            if (lease)
              dev.hostname = lease.hostname;

            return dev;
          });
        });

        this.$wireless.getAssoclist().then(assoclist => {
          this.assoclist = assoclist.map(sta => {
            const lease = leasesMap[sta.mac.toLowerCase()];
            if (lease)
              sta.host = `${lease.hostname} (${lease.ipaddr})`
            return sta;
          });
        });
      });

      this.$ubus.call('oui.network', 'dhcp6_leases').then(r => {
        this.leases6 = r.leases;
      });
    }
  }
}
</script>

<style lang="scss">
.oui-home-status-img {
  img {
    cursor: pointer;
  }
}
</style>
