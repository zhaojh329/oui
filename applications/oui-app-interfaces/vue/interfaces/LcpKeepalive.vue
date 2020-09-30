<template>
  <div>
    <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.LCP echo failure threshold')" name="keepalive_failure" placeholder="0" rules="uinteger" :load="loadKF" :save="saveKeepalive"/>
    <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.LCP echo interval')" name="keepalive_interval" placeholder="5" :rules="{type: 'uinteger', min: 1}" :load="loadKI" :save="saveKeepalive"/>
  </div>
</template>

<script>
export default {
  name: 'LcpKeepalive',
  props: {
    uciSection: Object
  },
  methods: {
    loadKF (self) {
      const [v] = (this.$uci.get('network', self.sid, 'keepalive') || '').split(' ')
      return v
    },
    loadKI (self) {
      const v = (this.$uci.get('network', self.sid, 'keepalive') || '').split(' ')
      return v[1] || ''
    },
    saveKeepalive (self) {
      const failure = self.ouiSection.get(self.sid, 'keepalive_failure')
      const interval = self.ouiSection.get(self.sid, 'keepalive_interval')
      if (!failure && !interval) return

      this.$uci.set('network', self.sid, 'keepalive', `${failure || 0} ${interval || 5}`)
    }
  }
}
</script>
