<template>
  <div class="oui-home">
    <el-row type="flex" justify="center" align="middle">
      <div class="header" @click="content = 'devices'">
        <img src="/icons/devices.png" />
      </div>
      <oui-line length="300px"></oui-line>
      <div class="header" @click="content = 'router'">
        <img src="/icons/router.png" />
      </div>
      <oui-line length="300px" :disconnect="!wanIsUp"></oui-line>
      <div class="header" @click="content = 'internet'">
        <img src="/icons/internet.png" />
      </div>
    </el-row>
    <el-row type="flex" justify="center" align="middle" style="margin-bottom: 15px">
      <img src="/icons/arrow_up.png" :style="{opacity: content !== 'devices' ? 0 : 1}" />
      <oui-line length="340px" style="opacity: 0"></oui-line>
      <img src="/icons/arrow_up.png" :style="{opacity: content !== 'router' ? 0 : 1}" />
      <oui-line length="340px" style="opacity: 0"></oui-line>
      <img src="/icons/arrow_up.png" :style="{opacity: content !== 'internet' ? 0 : 1}" />
    </el-row>
    <div v-show="content === 'devices'">
      <el-table :data="leases">
        <el-table-column :label="$t('Hostname')" prop="hostname"></el-table-column>
        <el-table-column :label="$t('IPv4-Address')" prop="ipaddr"></el-table-column>
        <el-table-column :label="$t('MAC-Address')" prop="macaddr"></el-table-column>
        <el-table-column :label="$t('Leasetime remaining')" :formatter="row => row.expires <= 0 ? $t('expired') : '%t'.format(row.expires)"></el-table-column>
      </el-table>
    </div>
    <div v-show="content === 'router'">
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
    </div>
    <div v-show="content === 'internet'">
      <el-row type="flex" justify="center">
        <CardList :title="$t('Network')" :list="waninfo" style="width: 600px"></CardList>
      </el-row>
    </div>
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
      content: 'router',
      sysinfo: [],
      waninfo: [],
      leases: [],
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
        this.leases = r.leases;
      });
    }
  }
}
</script>

<style lang="scss">
.oui-home {
  .header {
    cursor: pointer;
  }
}
</style>