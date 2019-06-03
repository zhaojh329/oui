<template>
  <uci-form config="system">
    <uci-section title="System Properties" name="system" typed>
      <uci-tabs :tabpanes="[['general', 'General Settings'], ['logging', 'Logging']]">
        <template v-slot:general>
          <uci-input label="Local Time" :on-load="localTime" readonly></uci-input>
          <uci-input label="Hostname" name="hostname" required></uci-input>
          <uci-list label="Timezone" name="zonename" initial="UTC" :options="zoneinfo"></uci-list>
        </template>
        <template v-slot:logging>
          <uci-input label="System log buffer size" name="log_size" placeholder="16"></uci-input>
          <uci-input label="External system log server" name="log_ip" placeholder="0.0.0.0"></uci-input>
          <uci-input label="External system log server port" name="log_port" placeholder="514"></uci-input>
          <uci-list label="External system log server protocol" name="log_proto" initial="udp" :options="logProtos"></uci-list>
          <uci-input label="Write system log to file" name="log_file"></uci-input>
          <uci-list label="Log output level" name="conloglevel" initial="7" :options="conlogLevels"></uci-list>
          <uci-list label="Cron Log Level" name="cronloglevel" initial="5" :options="cronlogLevels"></uci-list>
        </template>
      </uci-tabs>
    </uci-section>
    <uci-section title="Time Synchronization" name="ntp">
      <uci-switch label="Enable NTP client" name="enable" :uci="false" :on-load="ntpCliEnabled" :on-save="ntpCliSave"></uci-switch>
      <uci-dlist label="NTP server candidates" name="server"></uci-dlist>
    </uci-section>
  </uci-form>
</template>

<script>
import zoneinfo from '@/plugins/zoneinfo'

export default {
  data() {
    return {
      logProtos: [
        ['udp', 'UDP'],
        ['tcp', 'TCP']
      ],
      conlogLevels: [
        ['8', 'Debug'],
        ['7', 'Info'],
        ['6', 'Notice'],
        ['5', 'Warning'],
        ['4', 'Error'],
        ['3', 'Critical'],
        ['2', 'Alert'],
        ['1', 'Emergency']
      ],
      cronlogLevels: [
        ['5', 'Debug'],
        ['8', 'Normal'],
        ['9', 'Warning']
      ]
    }
  },
  computed: {
    zoneinfo() {
      return zoneinfo.map(item => [item[0]]);
    }
  },
  methods: {
    ntpCliSave(resolve, v) {
      if (v === '1') {
        this.$system.initStart('sysntpd').then(() => {
          this.$system.initEnable('sysntpd').then(() => {
            resolve();
          });
        });
      } else {
        this.$system.initStop('sysntpd').then(() => {
          this.$system.initDisable('sysntpd').then(() => {
            resolve();
          });
        });
      }
    },
    ntpCliEnabled(resolve) {
      this.$system.initEnabled('sysntpd').then(enabled => {
        resolve(enabled ? '1' : '0');
      });
    },
    localTime(resolve) {
      this.$system.getSystemInfo().then(r => {
        const localTime = new Date(r.localtime * 1000).toString();
        resolve(localTime);
      });
    }
  }
}
</script>
