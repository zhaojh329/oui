<template>
  <a-tabs>
    <a-tab-pane key="general" :tab="$t('General Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Start on boot')" name="auto" initial/>
      <proto :uci-section="uciSection"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.IPv4 address')" name="ipaddr" required rules="ip4addr" @change="ipChange"/>
      <oui-form-item-select :uci-section="uciSection" :label="$t('interfaces.IPv4 netmask')" name="netmask" allow-create @change="maskChange" :options="['255.255.255.0', '255.255.0.0', '255.0.0.0']" required rules="netmask4"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.IPv4 broadcast')" name="broadcast" rules="ip4addr" :placeholder="broadcast"/>
      <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.IPv4 gateway')" name="gateway" rules="ip4addr"/>
      <oui-form-item-list :uci-section="uciSection" :label="$t('interfaces.DNS servers')" name="dns" rules="ipaddr"/>
    </a-tab-pane>
    <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
      <oui-form-item-switch :uci-section="uciSection" :label="$t('interfaces.Use builtin IPv6-management')" name="delegate" initial/>
      <force-link :uci-section="uciSection"/>
      <override-mac :uci-section="uciSection"/>
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
import OverrideMac from '../OverrideMac.vue'
import OverrideMtu from '../OverrideMtu.vue'
import IfaceType from '../IfaceType.vue'
import Ifname from '../Ifname.vue'
import Zone from '../Zone.vue'

export default {
  name: 'ProtoStatic',
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
      ipaddr: '',
      netmask: '',
      bridged: this.uciSection.type === 'bridge'
    }
  },
  computed: {
    broadcast () {
      const i = this.ipaddr.split('.')
      const m = this.netmask.split('.')

      let I = 0
      let M = 0

      for (let n = 0; n < 4; n++) {
        i[n] = parseInt(i[n])
        m[n] = parseInt(m[n])

        if (isNaN(i[n]) || i[n] < 0 || i[n] > 255 ||
          isNaN(m[n]) || m[n] < 0 || m[n] > 255) { return }

        I |= (i[n] << ((3 - n) * 8))
        M |= (m[n] << ((3 - n) * 8))
      }

      const B = I | ~M

      return '%d.%d.%d.%d'.format((B >> 24) & 0xFF, (B >> 16) & 0xFF, (B >> 8) & 0xFF, (B >> 0) & 0xFF)
    }
  },
  methods: {
    ipChange (self) {
      this.ipaddr = self.model
    },
    maskChange (self) {
      this.netmask = self.model
    },
    ifaceTyoeChanged (self) {
      this.bridged = self.model === 'bridge'
    }
  }
}
</script>
