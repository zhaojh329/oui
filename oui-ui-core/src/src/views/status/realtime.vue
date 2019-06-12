<template>
  <el-tabs value="load">
    <el-tab-pane :label="$t('Load')" name="load">
      <e-charts :options="chartOptionLoad"></e-charts>
    </el-tab-pane>
    <el-tab-pane :label="$t('Traffic')" name="traffic">
      <el-tabs :value="firstDeviceName">
        <el-tab-pane v-for="dev in devices" :key="dev.name" :label="dev.name" :name="dev.name">
          <e-charts :options="dev.chart"></e-charts>
        </el-tab-pane>
      </el-tabs>
    </el-tab-pane>
     <el-tab-pane :label="$t('Connections')" name="connections">
       <e-charts :options="chartOptionCon"></e-charts>
    </el-tab-pane>
  </el-tabs>
</template>

<script>
import 'echarts/lib/chart/line'
import 'echarts/lib/component/title'
import 'echarts/lib/component/polar'
import 'echarts/lib/component/tooltip'
import 'echarts/lib/component/legendScroll'

export default {
  data() {
    return {
      devices: [],
      chartOptionLoad: {
        backgroundColor: '#2B2B2B',
        tooltip: {trigger: 'axis'},
        grid: {show: true},
        legend: {
          data: [],
          textStyle: {color: '#FFF'}
        },
        xAxis: {
          type: 'category',
          axisLine: {lineStyle: {color: '#FFF'}},
          axisTick: {show: false},
          axisLabel: {show: false},
          data: []
        },
        yAxis: {
          axisLine: {lineStyle: {color: '#FFF'}}
        },
        series: [
          {
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color: 'red'}},
            lineStyle: {normal: {color: 'red'}},
            areaStyle: {},
            data:[]
          },
          {
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color: '#FF6600'}},
            lineStyle: {normal: {color: '#FF6600'}},
            areaStyle: {},
            data:[]
          },
          {
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color:'#FFAA00'}},
            lineStyle: {normal: {color: '#FFAA00'}},
            areaStyle: {},
            data:[]
          }
        ]
      },
      chartOptionTraffic: {
        backgroundColor: '#2B2B2B',
        tooltip: {trigger: 'axis'},
        grid: {show: true},
        legend: {
          data: [this.$t('Inbound'), this.$t('Outbound')],
          textStyle: {color: '#FFF'}
        },
        xAxis: {
          type: 'category',
          axisLine: {lineStyle: {color: '#FFF'}},
          axisTick: {show: false},
          axisLabel: {show: false},
          data: []
        },
        yAxis: {
          axisLine: {lineStyle: {color: '#FFF'}}
        },
        series: [
          {
            name: this.$t('Inbound'),
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color: 'blue'}},
            lineStyle: {normal: {color: 'blue'}},
            areaStyle: {},
            data:[]
          },
          {
            name: this.$t('Outbound'),
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color: 'green'}},
            lineStyle: {normal: {color: 'green'}},
            areaStyle: {},
            data:[]
          }
        ]
      },
      chartOptionCon: {
        backgroundColor: '#2B2B2B',
        tooltip: {trigger: 'axis'},
        grid: {show: true},
        legend: {
          data: ['UDP', 'TCP', this.$t('Other')],
          textStyle: {color: '#FFF'}
        },
        xAxis: {
          type: 'category',
          axisLine: {lineStyle: {color: '#FFF'}},
          axisTick: {show: false},
          axisLabel: {show: false},
          data: []
        },
        yAxis: {
          axisLine: {lineStyle: {color: '#FFF'}}
        },
        series: [
          {
            name: 'UDP',
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color: 'blue'}},
            lineStyle: {normal: {color: 'blue'}},
            areaStyle: {},
            data:[]
          },
          {
            name: 'TCP',
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color: 'green'}},
            lineStyle: {normal: {color: 'green'}},
            areaStyle: {},
            data:[]
          },
          {
            name: this.$t('Other'),
            type: 'line',
            symbol: 'circle',
            itemStyle: {normal: {color:'red'}},
            lineStyle: {normal: {color: 'red'}},
            areaStyle: {},
            data:[]
          }
        ]
      }
    }
  },
  computed: {
    firstDeviceName() {
      if (this.devices.length === 0)
        return '';
      return this.devices[0].name;
    }
  },
  timers: {
    updateData: {time: 1000, autostart: true, repeat: true}
  },
  created() {
    const num = [1, 5, 15];
    for (let i = 0; i < 3; i++) {
      const legend = num[i] + this.$t('Minute Load');
      this.chartOptionLoad.legend.data.push(legend);
      this.chartOptionLoad.series[i].name = legend;
    }

    const now = new Date().getTime();
    for (let i = 0; i < 3 * 60; i++) {
      const ts = new Date(now + i * 1000).toTimeString().split(' ')[0];
      this.chartOptionLoad.xAxis.data.push(ts);
      this.chartOptionCon.xAxis.data.push(ts);
    }

    this.$ubus.call('network.device', 'status').then(devices => {
      this.devices = Object.keys(devices).filter(dev => dev !== 'lo').map(dev => {
        const chart = Object.assign({}, this.chartOptionTraffic);
        const now = new Date().getTime();
        for (let i = 0; i < 3 * 60; i++)
          chart.xAxis.data.push(new Date(now + i * 1000).toTimeString().split(' ')[0]);
        return {
          name: dev,
          chart: chart
        }
      });
    });
  },
  methods: {
    updateLoad() {
      if (this.chartOptionLoad.series[0].data.length === 3 * 60) {
        for (let i = 0; i < 3; i++)
          this.chartOptionLoad.series[i].data.shift();
        this.chartOptionLoad.xAxis.data.shift();
        this.chartOptionLoad.xAxis.data.push(new Date().toTimeString().split(' ')[0]);
      }

      this.$system.getSystemInfo().then(info => {
        const load = info.load;
        const l1 = (load[0] / 65535).toFixed(2) * 1;
        const l5 = (load[1] / 65535).toFixed(2) * 1;
        const l15 = (load[2] / 65535).toFixed(2) * 1;
        this.chartOptionLoad.series[0].data.push(l1);
        this.chartOptionLoad.series[1].data.push(l5);
        this.chartOptionLoad.series[2].data.push(l15);
      });
    },
    updateTraffic() {
      if (this.devices.length === 0)
        return;

      if (this.devices[0].chart.series[0].data.length === 3 * 60) {
        this.devices.forEach(dev => {
          dev.chart.series[0].data.shift();
          dev.chart.series[1].data.shift();
          dev.chart.xAxis.data.shift();
          dev.chart.xAxis.data.push(new Date().toTimeString().split(' ')[0]);
        });
      }

      this.$ubus.call('network.device', 'status').then(devices => {
        this.devices.forEach(dev => {
          const rx = devices[dev.name].statistics.rx_bytes / 1024;
          const tx = devices[dev.name].statistics.tx_bytes / 1024;

          if (!dev.last_rx) {
            dev.last_rx = rx;
            dev.last_tx = tx;
            return;
          }

          const rx_rate = (rx - dev.last_rx).toFixed(2) * 1;
          const tx_rate = (tx - dev.last_tx).toFixed(2) * 1;

          dev.chart.series[0].data.push(rx_rate);
          dev.chart.series[1].data.push(tx_rate);

          dev.last_rx = rx;
          dev.last_tx = tx;
        });
      });
    },
    updateCon() {
      if (this.chartOptionCon.series[0].data.length === 3 * 60) {
        for (let i = 0; i < 3; i++)
          this.chartOptionCon.series[i].data.shift();
        this.chartOptionCon.xAxis.data.shift();
        this.chartOptionCon.xAxis.data.push(new Date().toTimeString().split(' ')[0]);
      }

      this.$ubus.call('oui.network', 'conntrack_table').then(r => {
        let udp = 0;
        let tcp = 0;
        let oth = 0;

        r.entries.forEach(e => {
          if (e.protocol === 17)
            udp++;
          else if (e.protocol === 6)
            tcp++;
          else
            oth++;
        });

        this.chartOptionCon.series[0].data.push(udp);
        this.chartOptionCon.series[1].data.push(tcp);
        this.chartOptionCon.series[2].data.push(oth);
      });
    },
    updateData() {
      this.updateLoad();
      this.updateTraffic();
      this.updateCon();
    }
  }
}
</script>
