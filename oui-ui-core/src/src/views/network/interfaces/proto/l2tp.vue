<template>
  <div v-if="proto === 'l2tp'">
    <uci-option tab="general" type="input" :label="$t('Server')" name="server" depend="proto == 'l2tp'" required rules="host"></uci-option>
    <uci-option tab="general" type="input" :label="$t('PAP/CHAP username')" name="username" depend="proto == 'l2tp'"></uci-option>
    <uci-option tab="general" type="input" :label="$t('PAP/CHAP password')" name="password" password depend="proto == 'l2tp'"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use default gateway')" name="defaultroute" initial="1" depend="proto == 'l2tp'"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Use gateway metric')" name="metric" placeholder="0" depend="proto == 'l2tp' && defaultroute" rules="uinteger"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1" depend="proto == 'l2tp'"></uci-option>
    <uci-option tab="advanced" type="dlist" :label="$t('Use custom DNS servers')" name="dns" depend="proto == 'l2tp' && !peerdns" rules="ipaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Override MTU')" name="mtu" placeholder="1500" depend="proto == 'l2tp'" :rules="{type: 'uinteger', max: 9200}"></uci-option>
  </div>
</template>

<script>
export default {
  name: 'OuiProtoL2tp',
  props: {
    proto: String
  }
}
</script>
