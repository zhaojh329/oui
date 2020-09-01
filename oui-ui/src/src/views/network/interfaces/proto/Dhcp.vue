<template>
  <a-tabs>
    <a-tab-pane key="general" :tab="$t('General Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('Start on boot')" name="auto" initial/>
      <proto :uci-section="uciSection"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('Hostname to send when requesting DHCP')" name="hostname" :placeholder="hostname" rules="hostname"/>
    </a-tab-pane>
    <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('Use builtin IPv6-management')" name="delegate" initial/>
      <ForceLink :uci-section="uciSection"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('Use broadcast flag')" name="broadcast" :help="$t('Required for certain ISPs, e.g. Charter with DOCSIS3')"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('Use default gateway')" name="defaultroute" initial :help="$t('If unchecked, no default route is configured')"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('Use DNS servers advertised by peer')" name="peerdns" initial/>
      <oui-form-item-list :uci-section="uciSection" :label="$t('Use custom DNS servers')" name="dns" depend="!peerdns" rules="ipaddr"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('Client ID')" name="clientid" :help="$t('Client ID to send when requesting DHCP')"/>
      <oui-form-item-input :uci-section="uciSection" label="Vendor Class" name="vendorid" :help="$t('Vendor Class to send when requesting DHCP')"/>
      <override-mac :uci-section="uciSection"/>
      <override-mtu :uci-section="uciSection"/>
    </a-tab-pane>
    <a-tab-pane key="physical" :tab="$t('Physical Settings')">
      <IfaceType :uci-section="uciSection" @change="ifaceTyoeChanged"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('Enable STP')" name="stp" depend="type" :help="$t('Enables the Spanning Tree Protocol on this bridge')"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('Enable IGMP')" name="igmp_snooping" depend="type" :help="$t('Enables IGMP snooping on this bridge')"/>
      <ifname :uci-section="uciSection" :bridged="bridged"/>
    </a-tab-pane>
    <a-tab-pane key="firewall" :tab="$t('Firewall Settings')">
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
