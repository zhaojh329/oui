<template>
  <div>
    <uci-option-input tab="general" :label="$t('Server')" name="server" required rules="host"></uci-option-input>
    <uci-option-input tab="general" :label="$t('PAP/CHAP username')" name="username"></uci-option-input>
    <uci-option-input tab="general" :label="$t('PAP/CHAP password')" name="password" password></uci-option-input>
    <uci-option-switch tab="advanced" :label="$t('Use default gateway')" name="defaultroute" initial="1"></uci-option-switch>
    <uci-option-input tab="advanced" :label="$t('Use gateway metric')" name="metric" placeholder="0" depend="defaultroute" rules="uinteger"></uci-option-input>
    <uci-option-switch tab="advanced" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1"></uci-option-switch>
    <uci-option-dlist tab="advanced" :label="$t('Use custom DNS servers')" name="dns" depend="!peerdns" rules="ipaddr"></uci-option-dlist>
    <lcp-keepalive></lcp-keepalive>
    <uci-option-input tab="advanced" :label="$t('Inactivity timeout')" name="demand" placeholder="0" rules="uinteger"></uci-option-input>
    <override-mtu></override-mtu>
  </div>
</template>

<script>
import mixin from './proto'
import LcpKeepalive from './lcp-keepalive'
import OverrideMtu from './override-mtu'

export default {
  mixins: [mixin],
  components: {
    LcpKeepalive,
    OverrideMtu
  },
  data() {
    return {
      virtual: true,
      floating: true
    }
  }
}
</script>
