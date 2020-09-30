<template>
  <oui-form-item-input :uci-section="uciSection" :label="$t('interfaces.Override MAC address')" name="macaddr" :placeholder="placeholder" rules="macaddr"/>
</template>

<script>
export default {
  name: 'OverrideMac',
  props: {
    uciSection: Object
  },
  computed: {
    placeholder () {
      const iface = this.$network.getInterface(this.uciSection['.name'])
      const dev = iface.getDevice()
      if (dev) return dev.macaddr
      return ''
    }
  }
}
</script>
