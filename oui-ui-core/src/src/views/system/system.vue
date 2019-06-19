<template>
  <uci-form config="system">
    <uci-section :title="$t('System Properties')" type="system">
      <uci-tab :title="$t('General Settings')" name="general">
        <uci-option type="dummy" :label="$t('Local Time')" :load="localTime" name="__time"></uci-option>
        <uci-option type="input" :label="$t('Hostname')" name="hostname" required rules="hostname" :apply="updateHostname"></uci-option>
        <uci-option type="list" :label="$t('Timezone')" name="zonename" required initial="UTC" :options="zoneinfo" :save="saveTimezone"></uci-option>
      </uci-tab>
      <uci-tab :title="$t('Logging')" name="logging">
        <uci-option type="input" :label="$t('System log buffer size')" name="log_size" placeholder="16" description="kiB" :rules="{type: 'uinteger', min: 0, max: 128}"></uci-option>
        <uci-option type="input" :label="$t('External system log server')" name="log_ip" placeholder="0.0.0.0" rules="ip4addr"></uci-option>
        <uci-option type="input" :label="$t('External system log server port')" name="log_port" placeholder="514" rules="port"></uci-option>
        <uci-option type="list" :label="$t('External system log server protocol')" name="log_proto" initial="udp" :options="logProtos"></uci-option>
        <uci-option type="input" :label="$t('Write system log to file')" name="log_file"></uci-option>
        <uci-option type="list" :label="$t('Log output level')" name="conloglevel" initial="7" :options="conlogLevels" required></uci-option>
        <uci-option type="list" :label="$t('Cron Log Level')" name="cronloglevel" initial="5" :options="cronlogLevels" required></uci-option>
      </uci-tab>
    </uci-section>
    <uci-section :title="$t('Time Synchronization')" name="ntp">
      <uci-option type="switch" :label="$t('Enable NTP client')" name="enable" save="_" :load="ntpCliEnabled" :apply="ntpCliEnableApply"></uci-option>
      <uci-option type="switch" :label="$t('Provide NTP server')" name="enable_server" depend="enable"></uci-option>
      <uci-option type="dlist" :label="$t('NTP server candidates')" name="server" depend="enable"></uci-option>
    </uci-section>
  </uci-form>
</template>

<script>
import zoneinfo from '@/plugins/zoneinfo'

export default {
  data() {
    return {
      localTime: '',
      logProtos: [
        ['udp', 'UDP'],
        ['tcp', 'TCP']
      ],
      conlogLevels: [
        ['8', this.$t('Debug')],
        ['7', this.$t('Info')],
        ['6', this.$t('Notice')],
        ['5', this.$t('Warning')],
        ['4', this.$t('Error')],
        ['3', this.$t('Critical')],
        ['2', this.$t('Alert')],
        ['1', this.$t('Emergency')]
      ],
      cronlogLevels: [
        ['5', this.$t('Debug')],
        ['8', this.$t('Normal')],
        ['9', this.$t('Warning')]
      ]
    }
  },
  computed: {
    zoneinfo() {
      return zoneinfo.map(item => item[0]);
    }
  },
  timers: {
    loadLocalTime: {time: 3000, autostart: true, immediate: true, repeat: true}
  },
  methods: {
    loadLocalTime() {
      this.$system.getSystemInfo().then(r => {
        this.localTime = new Date(r.localtime * 1000).toString();
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

      this.$uci.set(config, sid, 'zonename', value);
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
    },
    updateHostname(resolve, value) {
      this.$store.commit('setHostname', value);
      resolve();
    }
  }
}
</script>
