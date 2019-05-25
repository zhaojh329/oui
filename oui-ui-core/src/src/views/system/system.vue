<template>
  <UciMap config="system">
    <UciSection name="system" title="System Properties" typed>
      <Tabs value="general">
        <TabPane label="General Settings" name="general">
          <UciInputValue title="Local Time" :value="localTime" readonly></UciInputValue>
          <UciInputValue name="hostname" title="Hostname"></UciInputValue>
          <UciListValue name="zonename" title="Timezone" :list="zoneinfo" default-val="UTC" @on-change="onTimezoneChange"></UciListValue>
        </TabPane>
        <TabPane label="Logging" name="logging">
          <UciInputValue name="log_size" title="System log buffer size"></UciInputValue>
          <UciInputValue name="log_ip" title="External system log server"></UciInputValue>
          <UciInputValue name="log_port" title="External system log server port"></UciInputValue>
          <UciListValue name="log_proto" title="External system log server protocol" :list="logProtos"></UciListValue>
          <UciInputValue name="log_file" title="Write system log to file"></UciInputValue>
          <UciListValue name="conloglevel" title="Log output level" :list="conlogLevels"></UciListValue>
          <UciListValue name="cronloglevel" title="Cron Log Level" :list="cronlogLevels"></UciListValue>
        </TabPane>
      </Tabs>
    </UciSection>
  </UciMap>
</template>

<script>

import zoneinfo from '@/plugins/zoneinfo'

export default {
  name: 'system',
  data() {
    return {
      localTime: '',
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
  timers: {
    getLocalTime: {time: 1000, autostart: true, repeat: true, immediate: true}
  },
  methods: {
    getLocalTime() {
      this.$system.getSystemInfo().then(r => {
        const date = new Date(r.localtime * 1000);
        this.localTime = date.toString()
      });
    },
    onTimezoneChange(data) {
      for (let i = 0; i < zoneinfo.length; i++) {
        if (zoneinfo[i][0] === data.value) {
          this.$uci.set(data.config, data.sid, 'timezone', zoneinfo[i][1]);
          return;
        }
      }
      this.$uci.set(data.config, data.sid, 'timezone', 'UTC');
    }
  }
}
</script>
