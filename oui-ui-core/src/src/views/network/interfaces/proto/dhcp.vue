<template>
  <div>
    <uci-option-input tab="general" :label="$t('Hostname to send when requesting DHCP')" name="hostname" :placeholder="hostname" rules="hostname"></uci-option-input>
    <uci-option-switch tab="advanced" :label="$t('Use broadcast flag')" name="broadcast" :description="$t('Required for certain ISPs, e.g. Charter with DOCSIS3')"></uci-option-switch>
    <uci-option-switch tab="advanced" :label="$t('Use default gateway')" name="defaultroute" initial="1" :description="$t('If unchecked, no default route is configured')"></uci-option-switch>
    <uci-option-switch tab="advanced" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial="1"></uci-option-switch>
    <uci-option-dlist tab="advanced" :label="$t('Use custom DNS servers')" name="dns" depend="!peerdns" rules="ipaddr"></uci-option-dlist>
    <uci-option-input tab="advanced" :label="$t('Client ID')" name="clientid" :description="$t('Client ID to send when requesting DHCP')"></uci-option-input>
    <uci-option-input tab="advanced" label="Vendor Class" name="vendorid" :description="$t('Vendor Class to send when requesting DHCP')"></uci-option-input>
    <override-mac></override-mac>
    <override-mtu></override-mtu>
    <uci-option-input tab="advanced" :label="$t('Override MTU')" name="mtu" placeholder="1500" :rules="{type: 'uinteger', max: 9200}"></uci-option-input>
  </div>
</template>

<script>
import mixin from './proto'
import OverrideMac from './override-mac'
import OverrideMtu from './override-mtu'

export default {
  mixins: [mixin],
  inject: ['uciSection'],
  data() {
    return {
      virtual: false,
      floating: false,
      macaddr: ''
    }
  },
  components: {
    OverrideMac,
    OverrideMtu
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
