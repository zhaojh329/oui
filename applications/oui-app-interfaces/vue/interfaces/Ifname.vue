<template>
  <oui-form-item-select :uci-section="uciSection" :label="$t('interfaces.Interface')" name="ifname" :options="devices" :multiple="multiple"/>
</template>

<script>
export default {
  name: 'Ifname',
  props: {
    uciSection: Object,
    bridged: Boolean
  },
  data () {
    return {
      devices: []
    }
  },
  computed: {
    multiple () {
      if (this.bridged) return true
      return false
    }
  },
  created () {
    this.$network.load().then(() => {
      this.devices = this.$network.getDevices().map(d => d.name)
    })
  }
}
</script>
