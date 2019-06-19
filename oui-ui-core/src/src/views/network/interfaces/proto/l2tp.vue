<template>
  <div v-if="proto === 'l2tp'">
    <uci-option-input tab="general" :label="$t('Server')" name="server" depend="proto == 'l2tp'" required rules="host"></uci-option-input>
    <uci-option-input tab="general" :label="$t('PAP/CHAP username')" name="username" depend="proto == 'l2tp'"></uci-option-input>
    <uci-option-input tab="general" :label="$t('PAP/CHAP password')" name="password" password depend="proto == 'l2tp'"></uci-option-input>
    <uci-option-switch tab="advanced" :label="$t('Use default gateway')" name="defaultroute" initial="1" depend="proto == 'l2tp'"></uci-option-switch>
    <uci-option-input tab="advanced" :label="$t('Use gateway metric')" name="metric" placeholder="0" depend="proto == 'l2tp' && defaultroute" rules="uinteger"></uci-option-input>
    <uci-option-switch tab="advanced" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1" depend="proto == 'l2tp'"></uci-option-switch>
    <uci-option-dlist tab="advanced" :label="$t('Use custom DNS servers')" name="dns" depend="proto == 'l2tp' && !peerdns" rules="ipaddr"></uci-option-dlist>
    <uci-option-input tab="advanced" :label="$t('Override MTU')" name="mtu" placeholder="1500" depend="proto == 'l2tp'" :rules="{type: 'uinteger', max: 9200}"></uci-option-input>
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
