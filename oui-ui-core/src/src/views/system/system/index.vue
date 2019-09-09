<template>
  <uci-form config="system" tabbed>
    <uci-section :title="$t('General Settings')" type="system">
        <uci-option-dummy :label="$t('Local Time')" :load="localTime" name="__time"></uci-option-dummy>
        <uci-option-input type="input" :label="$t('Hostname')" name="hostname" required rules="hostname" @applied="updateHostname"></uci-option-input>
        <uci-option-list :label="$t('Timezone')" name="zonename" required initial="UTC" :options="zoneinfo" :save="saveTimezone"></uci-option-list>
    </uci-section>
    <uci-section :title="$t('Logging')" type="system">
      <uci-option-input :label="$t('System log buffer size')" name="log_size" placeholder="16" append="kiB" :rules="{type: 'uinteger', min: 0, max: 128}"></uci-option-input>
      <uci-option-input :label="$t('External system log server')" name="log_ip" placeholder="0.0.0.0" rules="ip4addr"></uci-option-input>
      <uci-option-input :label="$t('External system log server port')" name="log_port" placeholder="514" rules="port"></uci-option-input>
      <uci-option-list :label="$t('External system log server protocol')" name="log_proto" initial="udp" :options="logProtos" required></uci-option-list>
      <uci-option-file :label="$t('Write system log to file')" name="log_file"></uci-option-file>
      <uci-option-list :label="$t('Log output level')" name="conloglevel" initial="7" :options="conlogLevels" required></uci-option-list>
      <uci-option-list :label="$t('Cron Log Level')" name="cronloglevel" initial="5" :options="cronlogLevels" required></uci-option-list>
    </uci-section>
    <uci-section :title="$t('Time Synchronization')" name="ntp">
      <uci-option-switch :label="$t('Enable NTP client')" name="enable" save="" :load="ntpCliEnabled" :apply="ntpCliEnableApply"></uci-option-switch>
      <uci-option-switch :label="$t('Provide NTP server')" name="enable_server" depend="enable"></uci-option-switch>
      <uci-option-dlist :label="$t('NTP server candidates')" name="server" depend="enable"></uci-option-dlist>
    </uci-section>
  </uci-form>
</template>

<script>
import zoneinfo from './zoneinfo'

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
    saveTimezone(sid, value) {
      let timezone = 'UTC';

      for (let i = 0; i < zoneinfo.length; i++) {
        if (zoneinfo[i][0] === value) {
          timezone = zoneinfo[i][1];
          break;
        }
      }

      this.$uci.set('system', sid, 'zonename', value);
      this.$uci.set('system', sid, 'timezone', timezone);
    },
    ntpCliEnabled() {
      return this.$system.initEnabled('sysntpd');
    },
    ntpCliEnableApply(v) {
      return new Promise(resolve => {
        if (v) {
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
      });
    },
    updateHostname(value) {
      this.$store.commit('setHostname', value);
    }
  }
}
</script>
