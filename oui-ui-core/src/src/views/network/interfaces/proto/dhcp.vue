<template>
  <div v-if="proto === 'dhcp'">
    <uci-option tab="general" type="input" :label="$t('Hostname')" name="hostname" depend="proto == 'dhcp'" :placeholder="hostname" rules="hostname"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use broadcast flag')" name="broadcast" depend="proto == 'dhcp'"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use default gateway')" name="defaultroute" initial="1" depend="proto == 'dhcp'"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1" depend="proto == 'dhcp'"></uci-option>
    <uci-option tab="advanced" type="dlist" :label="$t('Use custom DNS servers')" name="dns" depend="proto == 'dhcp' && !peerdns" rules="ipaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Client ID')" name="clientid" depend="proto == 'dhcp'"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Vendor Class')" name="vendorid" depend="proto == 'dhcp'"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Override MTU')" name="mtu" placeholder="1500" depend="proto == 'dhcp'" :rules="{type: 'uinteger', max: 9200}"></uci-option>
  </div>
</template>

<script>
export default {
  name: 'OuiProtoDhcp',
  props: {
    proto: String
  },
  computed: {
    hostname() {
      return this.$store.state.hostname;
    }
  }
}
</script>
