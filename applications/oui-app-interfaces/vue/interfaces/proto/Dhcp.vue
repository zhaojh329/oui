<template>
  <a-tabs>
    <a-tab-pane key="general" :tab="$t('General Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Start on boot')" name="auto" initial/>
      <proto :uci-section="uciSection"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.Hostname to send when requesting DHCP')" name="hostname" :placeholder="hostname" rules="hostname"/>
    </a-tab-pane>
    <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use builtin IPv6-management')" name="delegate" initial/>
      <ForceLink :uci-section="uciSection"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use broadcast flag')" name="broadcast" :help="$t('interfaces.UbfHelp')"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use default gateway')" name="defaultroute" initial :help="$t('interfaces.UdgHelp')"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use DNS servers advertised by peer')" name="peerdns" initial/>
      <oui-form-item-list :uci-section="uciSection" :label="$t('interfaces.Use custom DNS servers')" name="dns" depend="!peerdns" rules="ipaddr"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.Client ID')" name="clientid" :help="$t('interfaces.Client ID to send when requesting DHCP')"/>
      <oui-form-item-input :uci-section="uciSection" label="interfaces.Vendor Class" name="vendorid" :help="$t('interfaces.Vendor Class to send when requesting DHCP')"/>
      <override-mac :uci-section="uciSection"/>
      <override-mtu :uci-section="uciSection"/>
    </a-tab-pane>
    <a-tab-pane key="physical" :tab="$t('interfaces.Physical Settings')">
      <IfaceType :uci-section="uciSection" @change="ifaceTyoeChanged"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Enable STP')" name="stp" depend="type" :help="$t('interfaces.Enables the Spanning Tree Protocol on this bridge')"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Enable IGMP')" name="igmp_snooping" depend="type" :help="$t('interfaces.Enables IGMP snooping on this bridge')"/>
      <ifname :uci-section="uciSection" :bridged="bridged"/>
    </a-tab-pane>
    <a-tab-pane key="firewall" :tab="$t('interfaces.Firewall Settings')">
      <zone :uci-section="uciSection"/>
    </a-tab-pane>
  </a-tabs>
</template>

<script>
import Proto from '../Proto.vue'
import ForceLink from '../ForceLink.vue'
import OverrideMac from '../OverrideMac.vue'
import OverrideMtu from '../OverrideMtu.vue'
import IfaceType from '../IfaceType.vue'
import Ifname from '../Ifname.vue'
import Zone from '../Zone.vue'

export default {
  name: 'ProtoDhcp',
  props: {
    uciSection: Object
  },
  components: {
    Proto,
    ForceLink,
    OverrideMac,
    OverrideMtu,
    IfaceType,
    Ifname,
    Zone
  },
  data () {
    return {
      bridged: this.uciSection.type === 'bridge'
    }
  },
  computed: {
    hostname () {
      return this.$store.state.hostname
    }
  },
  methods: {
    ifaceTyoeChanged (self) {
      this.bridged = self.model === 'bridge'
    }
  }
}
</script>
