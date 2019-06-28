<template>
  <uci-option-input tab="advanced" :label="$t('Override MAC address')" name="macaddr" :placeholder="placeholder" rules="macaddr"></uci-option-input>
</template>

<script>
export default {
  inject: ['uciSection'],
  data() {
    return {
      placeholder: ''
    }
  },
  created() {
    const iface = this.$network.getInterface(this.uciSection.name);
    const dev = iface.getDevice();
    if (dev)
      this.placeholder = dev.macaddr;
  }
}
</script>
