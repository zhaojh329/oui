<template>
  <div>
    <uci-option-input tab="advanced" :label="$t('LCP echo failure threshold')" name="_keepalive_failure" placeholder="0" rules="uinteger" :load="loadKF" :save="saveKeepalive" @change="onKFChange"></uci-option-input>
    <uci-option-input tab="advanced" :label="$t('LCP echo interval')" name="_keepalive_interval" placeholder="5" :rules="{type: 'uinteger', min: 1}" :load="loadKI" :save="saveKeepalive" @change="onKIChange"></uci-option-input>
  </div>
</template>

<script>
export default {
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
    loadKF(sid) {
      const [v] = (this.$uci.get('network', sid, 'keepalive') || '').split(' ');
      return v;
    },
    loadKI(sid) {
      const v = (this.$uci.get('network', sid, 'keepalive') || '').split(' ');
      return v[1] || '';
    },
    saveKeepalive(sid) {
      if (!this.keepalive.failure && !this.keepalive.interval)
        return;

      const failure = this.keepalive.failure || 0;
      const interval = this.keepalive.interval || 5;
      this.$uci.set('network', sid, 'keepalive', `${failure} ${interval}`);
    }
  }
}
</script>
