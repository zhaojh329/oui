<template>
  <v-layout>
    <v-layout column fill-height>
      <card-list title="Device Infomation" :list="info"></card-list>
      <v-card>
        <v-card-title primary-title>Internet Status</v-card-title>
        <v-divider></v-divider>
        <v-card-text>
          <v-container grid-list-md text-xs-center>
            <v-layout row wrap>
              <v-flex xs2>
                <v-icon large color="primary">router</v-icon>
              </v-flex>
              <v-flex xs8>
                <v-progress-linear :indeterminate="true" v-if="wanInfo.up"></v-progress-linear>
                <v-icon large color="red" v-else>close</v-icon>
              </v-flex>
              <v-flex xs2>
                <v-icon large color="primary">cloud</v-icon>
              </v-flex>
            </v-layout>
          </v-container>
          <card-list :list="wanInfo.list"></card-list>
        </v-card-text>
      </v-card>
    </v-layout>
    <v-layout column fill-height>
      <v-card style="margin: 10px;" v-for="item in graphInfo" :key="item.title">
        <v-card-text>
           <v-list :two-line="item.line == 2" :three-line="item.line == 3">
            <v-list-tile avatar>
              <v-list-tile-avatar>
                <v-icon color="primary" x-large>{{item.icon}}</v-icon>
              </v-list-tile-avatar>
              <v-list-tile-content>
                <v-list-tile-title>{{item.value}}</v-list-tile-title>
                <v-list-tile-title v-if="item.line == 3">
                  <v-progress-linear :value="valueDeterminate"></v-progress-linear>
                </v-list-tile-title>
                <v-list-tile-title>{{item.title}}</v-list-tile-title>
              </v-list-tile-content>
            </v-list-tile>
           </v-list>
        </v-card-text>
      </v-card>
    </v-layout>
  </v-layout>
</template>

<script>
  export default {
    data() {
      return {
        info: [],
        valueDeterminate: 50,
        memory: {
          total: 0,
          free: 0
        },
        load: 12.12,
        storage: 20.2,
        uptime: {
          v: 0
        },
        graphInfo: [],
        wanInfo: {
          up: false,
          list: []
        }
      }
    },
    mounted() {
      this.$system.getInfo().then(r => {
        this.info = [
          ['Model', r.model],
          ['Architecture', r.system],
          ['Firmware Version', r.release.revision],
          ['Kernel Version', r.kernel]
        ];

        let format = this.$options.filters['format'];

        this.graphInfo = [
          {title: 'Uptime', line: 2, icon: 'access_time', value: format(r.uptime, 't')},
          {title: 'Memory', line: 3, icon: 'memory', value: format(r.memory.free, 'm') + 'B/' + format(r.memory.total, 'm') + 'B'},
          {title: 'Load', line: 3, icon: 'battery_std', value: (r.load[0] / 65535 * 100).toFixed(2) + '%'},
          {title: 'Free Disk', line: 3, icon: 'sd_storage', value: format(r.root.free, 'm') + 'B/' + format(r.root.total, 'm') + 'B'},
        ];
      });

      this.$ubus.call('network.interface', 'status', {interface: 'wan'}).then(r => {
        this.wanInfo.up = r.up;
        this.wanInfo.list = [
          ['IP Address', r['ipv4-address'][0].address],
          ['Gateway', r.route[0].nexthop],
          ['DNS', r['dns-server'][0]]
        ];
      });
    }
  };
</script>
