<template>
  <div v-if="proto === 'pptp'">
    <uci-option tab="general" type="input" :label="$t('Server')" name="server" depend="proto == 'pptp'" required rules="host"></uci-option>
    <uci-option tab="general" type="input" :label="$t('PAP/CHAP username')" name="username" depend="proto == 'pptp'"></uci-option>
    <uci-option tab="general" type="input" :label="$t('PAP/CHAP password')" name="password" password depend="proto == 'pptp'"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use default gateway')" name="defaultroute" initial="1" depend="proto == 'pptp'"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Use gateway metric')" name="metric" placeholder="0" depend="proto == 'pptp' && defaultroute" rules="uinteger"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1" depend="proto == 'pptp'"></uci-option>
    <uci-option tab="advanced" type="dlist" :label="$t('Use custom DNS servers')" name="dns" depend="proto == 'pptp' && !peerdns" rules="ipaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('LCP echo failure threshold')" name="_keepalive_failure" placeholder="0" depend="proto == 'pptp'" rules="uinteger" :load="loadKF" :save="saveKeepalive" @change="onKFChange"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('LCP echo interval')" name="_keepalive_interval" placeholder="5" depend="proto == 'pptp'" :rules="{type: 'uinteger', min: 1}" :load="loadKI" :save="saveKeepalive" @change="onKIChange"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Inactivity timeout')" name="demand" placeholder="0" depend="proto == 'pptp'" rules="uinteger"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Override MTU')" name="mtu" placeholder="1500" depend="proto == 'pptp'" :rules="{type: 'uinteger', max: 9200}"></uci-option>
  </div>
</template>

<script>
export default {
  name: 'OuiProtoPptp',
  props: {
    proto: String
  },
  data() {
    return {
      keepalive: {
        failure: '',
        interval: ''
      }
    }
  },
  methods: {
    onKFChange(v) {
      this.keepalive.failure = v;
    },
    onKIChange(v) {
      this.keepalive.interval = v;
    },
    loadKF(resolve, sid) {
      const [v] = (this.$uci.get('network', sid, 'keepalive') || '').split(' ');
      resolve(v);
    },
    loadKI(resolve, sid) {
      const v = (this.$uci.get('network', sid, 'keepalive') || '').split(' ');
      resolve(v[1] || '');
    },
    saveKeepalive(config, sid) {
      if (!this.keepalive.failure && !this.keepalive.interval)
        return;

      const failure = this.keepalive.failure || 0;
      const interval = this.keepalive.interval || 5;
      this.$uci.set('network', sid, 'keepalive', `${failure} ${interval}`);
    }
  }
}
</script>
