<template>
  <uci-form config="system">
    <uci-section title="System Properties" type="system">
      <uci-tab title="General Settings" name="general">
        <uci-option type="dummy" label="Local Time" :load="localTime" name="__time"></uci-option>
        <uci-option type="input" label="Hostname" name="hostname" required rules="hostname"></uci-option>
        <uci-option type="list" label="Timezone" name="zonename" initial="UTC" :options="zoneinfo" :save="saveTimezone"></uci-option>
      </uci-tab>
      <uci-tab title="Logging" name="logging">
        <uci-option type="input" label="System log buffer size" name="log_size" placeholder="16"></uci-option>
        <uci-option type="input" label="External system log server" name="log_ip" placeholder="0.0.0.0"></uci-option>
        <uci-option type="input" label="External system log server port" name="log_port" placeholder="514"></uci-option>
        <uci-option type="list" label="External system log server protocol" name="log_proto" initial="udp" :options="logProtos"></uci-option>
        <uci-option type="input" label="Write system log to file" name="log_file"></uci-option>
        <uci-option type="list" label="Log output level" name="conloglevel" initial="7" :options="conlogLevels"></uci-option>
        <uci-option type="list" label="Cron Log Level" name="cronloglevel" initial="5" :options="cronlogLevels"></uci-option>
      </uci-tab>
    </uci-section>
    <uci-section title="Time Synchronization" name="ntp">
      <uci-option type="switch" label="Enable NTP client" name="enable" :uci="false" :load="ntpCliEnabled" :apply="ntpCliEnableApply"></uci-option>
      <uci-option type="switch" label="Provide NTP server" name="enable_server" depend="enable"></uci-option>
      <uci-option type="dlist" label="NTP server candidates" name="server" depend="enable"></uci-option>
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
      return zoneinfo.map(item => item[0]);
    }
  },
  methods: {
    localTime(resolve) {
      this.$system.getSystemInfo().then(r => {
        const localTime = new Date(r.localtime * 1000).toString();
        resolve(localTime);
      });
    },
    saveTimezone(config, sid, name, value) {
      let timezone = 'UTC';

      for (let i = 0; i < zoneinfo.length; i++) {
        if (zoneinfo[i][0] === value) {
          timezone = zoneinfo[i][1];
          break;
        }
      }

      this.$uci.set(config, sid, 'timezone', timezone);
    },
    ntpCliEnabled(resolve) {
      this.$system.initEnabled('sysntpd').then(enabled => {
        resolve(enabled ? '1' : '0');
      });
    },
    ntpCliEnableApply(resolve, v) {
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
    }
  }
}
</script>
