<template>
  <a-tabs>
    <a-tab-pane key="general" :tab="$t('General Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Start on boot')" name="auto" initial/>
      <proto :uci-section="uciSection"/>
    </a-tab-pane>
    <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use builtin IPv6-management')" name="delegate" initial/>
      <force-link :uci-section="uciSection"/>
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
import Ifname from '../Ifname.vue'
import Zone from '../Zone.vue'

export default {
  name: 'ProtoNone',
  props: {
    uciSection: Object
  },
  components: {
    Proto,
    ForceLink,
    IfaceType,
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
