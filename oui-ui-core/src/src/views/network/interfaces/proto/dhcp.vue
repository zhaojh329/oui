<template>
  <div v-if="proto === 'dhcp'">
    <uci-option tab="general" type="input" :label="$t('Hostname to send when requesting DHCP')" name="hostname" depend="proto == 'dhcp'" :placeholder="hostname" rules="hostname"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use broadcast flag')" name="broadcast" depend="proto == 'dhcp'" :description="$t('Required for certain ISPs, e.g. Charter with DOCSIS3')"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use default gateway')" name="defaultroute" initial="1" depend="proto == 'dhcp'" :description="$t('If unchecked, no default route is configured')"></uci-option>
    <uci-option tab="advanced" type="switch" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1" depend="proto == 'dhcp'"></uci-option>
    <uci-option tab="advanced" type="dlist" :label="$t('Use custom DNS servers')" name="dns" depend="proto == 'dhcp' && !peerdns" rules="ipaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Client ID')" name="clientid" depend="proto == 'dhcp'" :description="$t('Client ID to send when requesting DHCP')"></uci-option>
    <uci-option tab="advanced" type="input" label="Vendor Class" name="vendorid" depend="proto == 'dhcp'" :description="$t('Vendor Class to send when requesting DHCP')"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Override MAC address')" name="macaddr" :placeholder="macaddr" depend="proto == 'dhcp'" rules="macaddr"></uci-option>
    <uci-option tab="advanced" type="input" :label="$t('Override MTU')" name="mtu" placeholder="1500" depend="proto == 'dhcp'" :rules="{type: 'uinteger', max: 9200}"></uci-option>
  </div>
</template>

<script>
export default {
  name: 'OuiProtoDhcp',
  inject: ['uciSection'],
  props: {
    proto: String
  },
  data() {
    return {
      macaddr: ''
    }
  },
  computed: {
    hostname() {
      return this.$store.state.hostname;
    },
    interfaceName() {
      return this.uciSection.name;
    }
  },
  created() {
    const iface = this.$network.getInterface(this.uciSection.name);
    const dev = iface.getDevice();
    if (dev)
      this.macaddr = dev.macaddr;
  }
}
</script>
