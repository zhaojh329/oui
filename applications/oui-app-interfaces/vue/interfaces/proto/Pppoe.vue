<template>
  <a-tabs>
    <a-tab-pane key="general" :tab="$t('General Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Start on boot')" name="auto" initial/>
      <proto :uci-section="uciSection"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.PAP/CHAP password')" name="password" type="password"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.Access Concentrator')" name="ac" :placeholder="$t('auto')"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.Service Name')" name="service" :placeholder="$t('auto')"/>
    </a-tab-pane>
    <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use builtin IPv6-management')" name="delegate" initial/>
      <force-link :uci-section="uciSection"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use default gateway')" name="defaultroute" initial :help="$t('interfaces.UdgHelp')"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.Use gateway metric')" name="metric" placeholder="0" rules="uinteger" depend="defaultroute"/>
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use DNS servers advertised by peer')" name="peerdns" initial="1"/>
      <OuiFormItemList :uci-section="uciSection" :label="$t('interfaces.Use custom DNS servers')" name="dns" depend="!peerdns" rules="ipaddr"/>
      <lcp-keepalive :uci-section="uciSection"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.Inactivity timeout')" name="demand" placeholder="0" rules="uinteger"/>
      <override-mtu :uci-section="uciSection"/>
    </a-tab-pane>
    <a-tab-pane key="physical" :tab="$t('interfaces.Physical Settings')">
      <iface-type :uci-section="uciSection" @change="ifaceTyoeChanged"/>
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
import IfaceType from '../IfaceType.vue'
import LcpKeepalive from '../LcpKeepalive.vue'
import OverrideMtu from '../OverrideMtu.vue'
import Ifname from '../Ifname.vue'
import Zone from '../Zone.vue'

export default {
  name: 'ProtoPppoe',
  props: {
    uciSection: Object
  },
  components: {
    Proto,
    IfaceType,
    ForceLink,
    LcpKeepalive,
    OverrideMtu,
    Ifname,
    Zone
  },
  data () {
    return {
      bridged: this.uciSection.type === 'bridge'
    }
  },
  methods: {
    ifaceTyoeChanged (self) {
      this.bridged = self.model === 'bridge'
    }
  }
}
</script>
